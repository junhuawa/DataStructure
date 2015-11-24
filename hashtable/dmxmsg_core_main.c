/*****************************************************************************
 * *
 * *      MODULE:     dmxmsg_core_main.c
 * *
 * ***************************************************************************
 * *
 * *      AUTHOR:     Lauri Koponen
 * *      REVISED BY:
 * *
 * *      Copyright (c) 2008 Nokia Siemens Networks
 * ***************************************************************************
 * *
 * *      Linux kernel DMX messaging core module
 * *
 *****************************************************************************/

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <linux/version.h>

/* debug stuff */
#define DEBUG_LEVEL 0

#if DEBUG_LEVEL == 0
#define DPRINTF(lvl, fmt...) do { } while(0)
#else
#define DPRINTF(lvl, fmt...)      \
    do {                          \
        if(lvl <= DEBUG_LEVEL) {  \
          printk(fmt);            \
        }                         \
    } while(0)
#endif


#define BUFFER_BOTTOM_SIZE 48

#include "dmx_types.h"
#include "dmxmsg.h"

/* from dmxmsg_core_proc.c */
extern int co_unit_address;
extern int own_unit_state;
extern int co_unit_state;
extern int separate;
extern int trial;
extern int max_msg_queue_size;
extern int default_msg_queue_size;
extern int debugflag;
extern spinlock_t logaddr_table_lock;

static int client_table_used_count;
static int msg_seq_number = 0;
static int physaddr = -1;

int dmxmsg_own_phys_addr;
int dmxmsg_own_log_addr = COMPUTER_T_LOG_INVALID_C;

int dmxmsg_proc_create(void);
void dmxmsg_proc_delete(void);

module_param(physaddr, int, S_IRUGO);
MODULE_PARM_DESC(physaddr, "Own unit physical address");

/* CLIENT_TABLE_SIZE must be 1<<x */
#define CLIENT_TABLE_SIZE  32768
#define CLIENT_RELEASED__C  (struct dmxmsg_client*)-1
#define CLIENT_UNUSED__C  (struct dmxmsg_client*)0

static struct dmxmsg_post_office *current_post_office;

/* these are not static due to use by dmxmsg_core_proc.c */
struct list_head client_list;
struct dmxmsg_client *client_table[CLIENT_TABLE_SIZE];
spinlock_t client_table_lock;

/* counter for unnamed pid allocation */
static process_id_t free_process_id = 1; /* wrap from 0xFFFE to 1 */
static spinlock_t free_process_id_lock;
static spinlock_t pid_registration_lock;
static spinlock_t msg_seq_number_lock;

struct monitor {
    int active;
    struct dmxmsg_monitor *info;
};

#define MONITOR_TABLE_SIZE 4

spinlock_t monitor_table_lock;
struct monitor monitor_table[MONITOR_TABLE_SIZE];
int monitor_count;

struct dmxmsg_stats  dmxmsg_stats;

#define MSG_SOURCE_POST_OFFICE__C  0
#define MSG_SOURCE_LOCAL__C    1

/* judge unit is separated or not */
int dmxmsg_is_unit_separated(void)
{
    return separate;
}

static void fill_buffer_bottom(struct dmxmsg_client *client, struct sk_buff *skb);
static struct dmxmsg_client *find_client(struct dmxmsg_pid *pid);
static void handle_msg(struct sk_buff *skb, int source);
static int send_to_post_office(struct sk_buff *skb, computer_t *dst, int dst_count);
static void _monitor(struct sk_buff *skb, mon_points_t points);


inline static void monitor(struct sk_buff *skb, mon_points_t points)
{
    if(likely(monitor_count == 0)) /* atomic */
        return;
    _monitor(skb, points);
}

static int pid_hashfn(unsigned short family, unsigned short process_id)
{
    unsigned long h = 5381;

    h = h + (family >> 8);
    h = (h << 5) + h + (family & 255);
    h = (h << 5) + h + (process_id >> 8);
    h = (h << 5) + h + (process_id & 255);

    return h & (CLIENT_TABLE_SIZE - 1);
}


struct sk_buff *dmxmsg_alloc_msg(unsigned short length, gfp_t gfp_mask)
{
    struct sk_buff *skb;

    skb = alloc_skb(length + BUFFER_BOTTOM_SIZE, gfp_mask);
    if(!skb)
        return 0;

    skb_reserve(skb, BUFFER_BOTTOM_SIZE);
    skb_put(skb, length);
    return skb;
}

void dmxmsg_resize_msg(struct sk_buff *skb, unsigned short length)
{
    skb_trim(skb, length + BUFFER_BOTTOM_SIZE);
}

void dmxmsg_free_msg(struct sk_buff **skb)
{
    kfree_skb(*skb);
    *skb = 0;
}

static struct dmxmsg_pid_list *alloc_pid_list(struct dmxmsg_pid *pid)
{
    struct dmxmsg_pid_list *list;

    list = kmalloc(sizeof(struct dmxmsg_pid_list), GFP_ATOMIC);
    if(list == 0)
        return 0;

    INIT_LIST_HEAD(&list->list);
    list->pid = *pid;
    return list;
}

static void release_pid_list(struct dmxmsg_pid_list *pid_list)
{
    kfree(pid_list);
}


/* check if client owns given pid. client->lock must be held during this call. */
static int matching_pid(struct dmxmsg_client *client, struct dmxmsg_pid *pid)
{
    struct list_head *ptr;
    struct dmxmsg_pid_list *entry;

    list_for_each(ptr, &client->pid_list) {
        entry = list_entry(ptr, struct dmxmsg_pid_list, list);
        if(entry->pid.family == pid->family
           && entry->pid.process_id == pid->process_id
           &&entry->pid.focus == pid->focus)
            return 1;
    }
    return 0;
}

/* insert entry into empty slot in client table */
static void insert_client_pid(struct dmxmsg_pid *pid, struct dmxmsg_client *client)
{
    int h;
    int hash = pid_hashfn(pid->family, pid->process_id);

    spin_lock_bh(&client_table_lock);
    while(T) {
        h = hash & (CLIENT_TABLE_SIZE - 1);
        if((client_table[h] == CLIENT_UNUSED__C )
            || (client_table[h] == CLIENT_RELEASED__C )){
            client_table[h] = client;
            client_table_used_count++;
            break;
        }
        hash += 17; /* TBD: make up something better */
    }
    spin_unlock_bh(&client_table_lock);
}

/* delete entry from client table */
static void remove_client_pid(struct dmxmsg_pid *pid, struct dmxmsg_client *client)
{
    int h;
    int hash = pid_hashfn(pid->family, pid->process_id);
  
    spin_lock_bh(&client_table_lock);
    while(T) {
        h = hash & (CLIENT_TABLE_SIZE - 1);
        if(client_table[h] == CLIENT_UNUSED__C) {
            break;
        }
        if(client_table[h] != CLIENT_RELEASED__C) {
            if(client_table[h] == client) {
                client_table[h] = CLIENT_RELEASED__C;
                client_table_used_count--;
                break;
            }
        }
        hash += 17; /* TBD: make up something better */
    }
    spin_unlock_bh(&client_table_lock);
}

int dmxmsg_register_client(struct dmxmsg_client *client)
{
    /* TBD */
    return 0;
}

/* must be called with client->lock held */
static int dmxmsg_unregister_client(struct dmxmsg_client *client)
{
    struct list_head *ptr;
    struct list_head *next;
    struct dmxmsg_pid_list *entry;
    struct sk_buff *skb;
    int i;

    list_for_each_safe(ptr, next, &client->pid_list) {
        entry = list_entry(ptr, struct dmxmsg_pid_list, list);
        DPRINTF(3, "removing entry %x:%x:%x\n", entry->pid.family,entry->pid.process_id, entry->pid.focus);
        list_del(&entry->list);
        remove_client_pid(&entry->pid, client);
        release_pid_list(entry);
    }

    /* empty message queue */
    for(i = 0; i < MSG_PRIORITY_COUNT__C; i++) {
        while((skb = skb_dequeue(&client->msg_queue[i])) != 0) {
            atomic_sub(skb->len, &dmxmsg_stats.memory_in_use);
            atomic_dec(&dmxmsg_stats.buffers_in_use);
            kfree_skb(skb);
        }
    }

    client->active_pid.family = 0xFFFF;
    client->active_pid.process_id = 0xFFFF;
    client->active_pid.focus = 0xFF;
    return 0;
}

/* forcefully unregister a pid */
int dmxmsg_force_unregister_pid(struct dmxmsg_pid *pid)
{
    int lock_released = 0;
    struct list_head *ptr;
    struct list_head *next;
    struct dmxmsg_pid_list *entry;

    struct dmxmsg_client *owner;

    DPRINTF(3, "trying to unregister pid %x:%x:%x\n",
      entry->pid.family, entry->pid.process_id, entry->pid.focus);

    owner = find_client(pid);
    if(!owner) {
        DPRINTF(3, "...not found\n");
        return -ENXIO;
    }

    spin_lock_bh(&owner->lock);
    list_for_each_safe(ptr, next, &owner->pid_list) {
        entry = list_entry(ptr, struct dmxmsg_pid_list, list);
        if(entry->pid.family == pid->family
           && entry->pid.process_id == pid->process_id
           && entry->pid.focus == pid->focus) {
            DPRINTF(3, "removing entry %x:%x:%x\n",
                    entry->pid.family, entry->pid.process_id, entry->pid.focus);
            list_del(&entry->list);
            /* release the spinlock so remove_client_pid cannot deadlock */
            spin_unlock_bh(&owner->lock);
            lock_released = 1;
            remove_client_pid(&entry->pid, owner);
            release_pid_list(entry);
            break;
        }
    }

    if(!lock_released)
        spin_unlock_bh(&owner->lock);

    dmxmsg_release_client(owner);
    return 0;
}


struct dmxmsg_client *dmxmsg_alloc_client(struct dmxmsg_client_ops *ops,
            pid_t pid, char *name)
{
    int i;
    struct dmxmsg_client *client;

    client = kmalloc(sizeof(struct dmxmsg_client), GFP_KERNEL);
    if(client == 0)
        return 0;

    client->ops = ops;
    client->refcount = 1;
    client->pid = pid;
    client->name = name;
    client->active_pid.family = 0xFFFF;
    client->active_pid.process_id = 0xFFFF;
    client->active_pid.focus = 0xFF;
    client->msg_queue_size = 0;
    client->msg_queue_limit = default_msg_queue_size;

    for(i = 0; i < MSG_PRIORITY_COUNT__C; i++)
        skb_queue_head_init(&client->msg_queue[i]);
    init_waitqueue_head(&client->msg_wait);
    INIT_LIST_HEAD(&client->pid_list);
    spin_lock_init(&client->lock);

    /* insert in client list */
    INIT_LIST_HEAD(&client->list);
    spin_lock_bh(&client_table_lock);
    list_add_tail(&client->list, &client_list);
    spin_unlock_bh(&client_table_lock);

    return client;
}

void dmxmsg_set_queue_size(struct dmxmsg_client *client, int limit)
{
    if(limit > max_msg_queue_size)
        limit = max_msg_queue_size;

    spin_lock_bh(&client->lock);
    client->msg_queue_limit = limit;
    spin_unlock_bh(&client->lock);
}

void dmxmsg_hold_client(struct dmxmsg_client *client)
{
    spin_lock_bh(&client->lock);
    client->refcount++;
    spin_unlock_bh(&client->lock);
}

void dmxmsg_release_client(struct dmxmsg_client *client)
{
    spin_lock_bh(&client->lock);
    client->refcount--;

    DPRINTF(3, "client->refcount=%d\n", client->refcount);

    if(client->refcount <= 0) {
        DPRINTF(3, "unregistering client\n");
        dmxmsg_unregister_client(client);
        spin_unlock_bh(&client->lock);
        spin_lock_bh(&client_table_lock);
        DPRINTF(3, "removing from client list\n");
        list_del(&client->list);
        spin_unlock_bh(&client_table_lock);
        DPRINTF(3, "releasing memory\n");
        kfree(client);
        return;
    }
    spin_unlock_bh(&client->lock);
}

int dmxmsg_register_pid(struct dmxmsg_client *client, struct dmxmsg_pid *pid)
{
    struct dmxmsg_pid_list *list_item;
    struct dmxmsg_pid reg_pid;
    struct dmxmsg_client *cl;
    int pid_scan_count = 0;

    /* somewhat arbitrary limit to stop insert_client_pid from crashing */
    if(client_table_used_count > CLIENT_TABLE_SIZE/2)
        return -ENOMEM;

    if(pid->family == 0xFFFF) {
        /* register unnamed pid */
        while(pid_scan_count < 0x10000) {
            reg_pid.family = 0x906;
            reg_pid.focus = 0;

            spin_lock_bh(&free_process_id_lock);
            reg_pid.process_id = free_process_id;
            free_process_id++;
            if(free_process_id == 0xFFFF)
                free_process_id = 1;
            spin_unlock_bh(&free_process_id_lock);

            pid_scan_count++;
            DPRINTF(3, "register_pid: looking for client %x:%x:%x\n",
                      reg_pid.family, reg_pid.process_id, reg_pid.focus);
            /* break if an unallocated pid was found */
            if(!(cl = find_client(&reg_pid))) {
                DPRINTF(3, "not found, ok!\n");
                break;
            }
            DPRINTF(3, "found, continuing\n");
            dmxmsg_release_client(cl);
        }

        if(pid_scan_count >= 0x10000) {
            /* ran out of free addresses */
            return -EADDRINUSE;
        }
    } else if(pid->process_id == 0xFFFF) {
        /* register an error handler */
        reg_pid = *pid;
    } else {
        reg_pid = *pid;
    }

    /* allow only one process at a time to register a pid */
    spin_lock_bh(&pid_registration_lock);
    cl = find_client(&reg_pid);
    if(cl) {
        /* PID has already been registered */
        spin_unlock_bh(&pid_registration_lock);
        dmxmsg_release_client(cl);
        return -EADDRINUSE;
    }

    spin_lock_bh(&client->lock);
    if(list_empty(&client->pid_list)) {
        client->active_pid = reg_pid;
    }
    list_item = alloc_pid_list(&reg_pid);
    list_add_tail(&list_item->list, &client->pid_list);
    spin_unlock_bh(&client->lock);

    insert_client_pid(&reg_pid, client);

    DPRINTF(1, "register_pid: %x:%x:%x (%p)\n", reg_pid.family, reg_pid.process_id
              , reg_pid.focus, client);

    spin_unlock_bh(&pid_registration_lock);
    return 0;
}

int dmxmsg_unregister_all_pids(struct dmxmsg_client *client)
{
    int r;

    spin_lock_bh(&client->lock);
    r = dmxmsg_unregister_client(client);
    spin_unlock_bh(&client->lock);

    return r;
}

int dmxmsg_get_active_pid(struct dmxmsg_client *client, struct dmxmsg_pid *pid)
{
    spin_lock_bh(&client->lock);
    *pid = client->active_pid;
    spin_unlock_bh(&client->lock);
    return 0;
}

int dmxmsg_set_active_pid(struct dmxmsg_client *client, struct dmxmsg_pid *pid)
{
    struct dmxmsg_client *owner;
  
    owner = find_client(pid);
    if(!owner)
        return -EINVAL;
    if(owner != client) {
        dmxmsg_release_client(owner);
        return -EINVAL;
    }

    spin_lock_bh(&client->lock);
    client->active_pid = *pid;
    spin_unlock_bh(&client->lock);

    dmxmsg_release_client(owner);
    return 0;
}

static void translate_msg(struct sk_buff *skb)
{
    int t;

    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);
    buffer_bottom_t *bb = (buffer_bottom_t*)skb->data;

    h->phys_computer = bb->phys_computer;

    t = h->computer;
    h->computer = bb->computer;
    bb->computer = t;

    t = h->family;
    h->family = bb->family;
    bb->family = t;

    t = h->process_id;
    h->process_id = bb->process_id;
    bb->process_id = t;

    t = h->focus;
    h->focus = bb->focus;
    bb->focus = t;
    bb->control &= ~MSG_CONTROL_T_SENDERS_VIEW_C;
}

static void send_registered_msg_ack(struct sk_buff *skb, dmx_error_t status)
{
    struct sk_buff *msg;
    buffer_bottom_t *ack_bb;
    single_comp_ack_s *ack;
    buffer_bottom_t *req_bb;
    msg_header_t *req_h;
  
    msg = dmxmsg_alloc_msg(sizeof(single_comp_ack_s), GFP_ATOMIC);
    ack = (single_comp_ack_s*)msg->data;
    req_bb = (buffer_bottom_t*)skb->data;
    req_h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);
    ack->msg_header.length = sizeof(single_comp_ack_s);
    ack->msg_header.computer = req_bb->phys_computer | (req_h->computer & 0x3000);
    ack->msg_header.family = req_bb->family;
    ack->msg_header.process_id = req_bb->process_id;
    ack->msg_header.focus = req_bb->focus;
    ack->msg_header.attributes = 1;
    ack->msg_header.group = 0;
    ack->msg_header.number = SINGLE_COMP_ACK_SN;
    ack->status = status;
    DPRINTF(1, "Registered msg ack to computer %x\n", ack->msg_header.computer);
    /* fill in buffer_bottom_t and set reg msg ack flag */
    fill_buffer_bottom(NULL, msg);
    ack_bb = (buffer_bottom_t*)msg->data;
    ack_bb->computer = (req_h->computer & 0x3FFF) | 0x8000;
    ack_bb->family = req_h->family;
    ack_bb->process_id = req_h->process_id;
    ack_bb->focus = req_h->focus;
    handle_msg(msg, MSG_SOURCE_LOCAL__C);
}

static int store_msg(struct dmxmsg_client *client, struct sk_buff *skb
    , int front)
{
    int temp;
    int r = 0;
    int priority;
    msg_attr_t attr;
    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);

    if(client->ops && client->ops->recv) {
        /* direct receiving */
        DPRINTF(3, "direct receive\n");
        monitor(skb, MON_POINTS_T_ARRIVE_C);
        monitor(skb, MON_POINTS_T_RECEIVE_C);
        /* strip buffer_bottom_t */
        skb_pull(skb, BUFFER_BOTTOM_SIZE);
        client->ops->recv(skb);
        r = 0;
        goto out;
    }

    attr = h->attributes;
    priority = attr & 3;

    spin_lock_bh(&client->lock);
    if(!(attr & MSG_ATTR_T_IMPORTANT_MSG_C)) {
        if(client->msg_queue_size + skb->len > client->msg_queue_limit) {
            spin_unlock_bh(&client->lock);
            r = -ENOSPC;
            goto out;
        }
    }

    client->msg_queue_size += skb->len;
    spin_lock_bh(&dmxmsg_stats.peak_lock);
    atomic_add(skb->len, &dmxmsg_stats.memory_in_use);
    temp = atomic_read(&dmxmsg_stats.memory_in_use);
    if(temp > dmxmsg_stats.memory_in_use_peak) {
        dmxmsg_stats.memory_in_use_peak = temp;
    }

    atomic_inc(&dmxmsg_stats.buffers_in_use);
    temp = atomic_read(&dmxmsg_stats.buffers_in_use);
    if(temp > dmxmsg_stats.buffers_in_use_peak) {
        dmxmsg_stats.buffers_in_use_peak = temp;
    }
  
    spin_unlock_bh(&dmxmsg_stats.peak_lock);
    monitor(skb, MON_POINTS_T_ARRIVE_C);

    if(front)
        skb_queue_head(&client->msg_queue[priority], skb);
    else
        skb_queue_tail(&client->msg_queue[priority], skb);

    spin_unlock_bh(&client->lock);
    DPRINTF(3, "waking up client\n");
    wake_up(&client->msg_wait);

out:
    return r;
}

static void fill_buffer_bottom(struct dmxmsg_client *client, struct sk_buff *skb)
{
    msg_header_t *h;
    buffer_bottom_t *bb;
    computer_t width;

    /* insert buffer_bottom_t */
    skb_push(skb, BUFFER_BOTTOM_SIZE);
    h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);
    bb = (buffer_bottom_t*)skb->data;
    bb->control = MSG_CONTROL_T_SENDERS_VIEW_C;
    spin_lock_bh(&msg_seq_number_lock);
    bb->sequence_number = msg_seq_number++;
    spin_unlock_bh(&msg_seq_number_lock);
    bb->hop_count = 0;
    bb->error_code = 0;
  
    if(client == NULL) {
        bb->family = EPOFFI_P; /* TBD: should this be 0 too? */
        bb->process_id = 0;
        bb->focus = 0;
    } else {
        spin_lock_bh(&client->lock);
        bb->family = client->active_pid.family;
        bb->process_id = client->active_pid.process_id;
        bb->focus = client->active_pid.focus;
        spin_unlock_bh(&client->lock);
    }

    if(h->computer == R_OWN_UNIT) {
        bb->computer = R_OWN_UNIT;
    } else if(h->computer == R_CO_UNIT) {
        bb->computer = R_CO_UNIT;
    } else if((h->computer & COMPUTER_T_INT_C) == COMPUTER_T_INT_LOG_C) {
        width = h->computer & COMPUTER_T_MASK_LOG_WIDTH_C;
        if(width == COMPUTER_T_WIDTH_WOSP_C) {
            bb->computer = dmxmsg_own_log_addr | COMPUTER_T_WIDTH_WOSP_C;
        } else {
            bb->computer = dmxmsg_own_log_addr;
        }
    } else {
        bb->computer = dmxmsg_own_phys_addr;
    }

    /* copy bus selection bits on physical address */
    if((h->computer & COMPUTER_T_INT_C) == COMPUTER_T_INT_PHYS_C) {
        bb->computer |= h->computer & COMPUTER_T_MB1_C;
    }

    bb->phys_computer = dmxmsg_own_phys_addr;
    h->phys_computer = dmxmsg_own_phys_addr;
    bb->next_phys_computer = h->computer; /* ? */
}

static void get_dest_pid(struct sk_buff *skb, struct dmxmsg_pid *pid)
{
    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);

    pid->family = h->family;
    pid->process_id = h->process_id;
    pid->focus = h->focus;
}

static struct dmxmsg_client *find_client(struct dmxmsg_pid *pid)
{
    int h;
    int hash = pid_hashfn(pid->family, pid->process_id);
    int first = hash;
    struct dmxmsg_client *r = 0;

    h = first;

    spin_lock_bh(&client_table_lock);
    do {
        if(client_table[h] == CLIENT_UNUSED__C) {
            break;
        }
        if(client_table[h] != CLIENT_RELEASED__C) {
            spin_lock_bh(&client_table[h]->lock);
            if(matching_pid(client_table[h], pid)) {
                client_table[h]->refcount++;
                spin_unlock_bh(&client_table[h]->lock);
                r = client_table[h];
                goto out;
            }
            spin_unlock_bh(&client_table[h]->lock);
        }
        hash += 17; /* TBD: make up something better */
        h = hash & (CLIENT_TABLE_SIZE - 1);
    } while(h != first);

out:
    spin_unlock_bh(&client_table_lock);
    return r;
}

static void init_monitor_table(void)
{
    memset(monitor_table, 0, sizeof(monitor_table));
    monitor_count = 0;
    spin_lock_init(&monitor_table_lock);
}

struct dmxmsg_monitor *dmxmsg_alloc_monitor(void)
{
    struct dmxmsg_monitor *monitor;

    monitor = kmalloc(sizeof(struct dmxmsg_monitor), GFP_KERNEL);
    if(monitor == 0)
        return 0;

    skb_queue_head_init(&monitor->msg_queue);
    init_waitqueue_head(&monitor->msg_wait);
    return monitor;
}

void dmxmsg_release_monitor(struct dmxmsg_monitor *monitor)
{
    dmxmsg_unregister_monitor(monitor);
    kfree(monitor);
}

int dmxmsg_register_monitor(struct dmxmsg_monitor *mon)
{
    int i;
    int r;

    r = -EBUSY;
    spin_lock_bh(&monitor_table_lock);
    for(i = 0; i < MONITOR_TABLE_SIZE; i++) {
        if(!monitor_table[i].active) {
            monitor_table[i].info = mon;
            monitor_table[i].active = T;
            monitor_count++;
            r = 0;
            break;
        }
    }
    spin_unlock_bh(&monitor_table_lock);
    return r;
}

static void clear_monitor_queue(struct dmxmsg_monitor *mon)
{
    struct sk_buff *skb;
    while((skb = skb_dequeue(&mon->msg_queue))) {
        kfree_skb(skb);
    }
}

int dmxmsg_unregister_monitor(struct dmxmsg_monitor *mon)
{
    int i;

    spin_lock_bh(&monitor_table_lock);
    for(i = 0; i < MONITOR_TABLE_SIZE; i++) {
        if(monitor_table[i].active) {
            if(monitor_table[i].info == mon) {
                monitor_table[i].active = F;
                monitor_count--;
                
                clear_monitor_queue(mon);
                break;
            }
        }
    }
    spin_unlock_bh(&monitor_table_lock);
    return 0;
}

static inline void monitor_push_stack(short **stack, int cond)
{
    (*stack)--;
    *(*stack) = cond;
}

static inline int monitor_peek_stack(short **stack)
{
    return *(*stack);
}


static inline int monitor_pop_stack(short **stack)
{
    int cond;
    cond = monitor_peek_stack(stack);
    (*stack)++;
    return cond;
}

#define NOT(x) (1 - (x))

static int monitor_filter_sub(struct dmxmsg_monitor *m, struct sk_buff *skb,
    short **cond_stack, signed char *v)
{
    expression_rec_t *expr;
    dword value, offset;
    int bottom = F;
    int cond, cond_store;
    int visit_left, visit_right, is_leaf;
    int cond_value;
    int start_cond = monitor_peek_stack(cond_stack);

filter_next:
    DPRINTF(3, KERN_EMERG "peeking\n");
    cond = monitor_peek_stack(cond_stack);
    DPRINTF(3, KERN_EMERG "next cond: %d\n", cond);
    if(cond == -1)
        return v[start_cond];

    cond_store = cond;

filter_again:
    cond_value = -1;
    switch(m->conditions[cond].comp_rec.check_type & 0xF0) {
        case 0:
            break;
        case MON_CHECK_TYPE_T_EXPR_C:
            /* expression */
            expr = (expression_rec_t*)&m->conditions[cond].expr_rec;
            DPRINTF(3, KERN_EMERG "expression\n");
            switch(expr->expr_type) {
                case EXPR_TYPE_T_NO_EXPR_C:
                    DPRINTF(3, KERN_EMERG "NO_EXPR\n");
                    visit_left = T;
                    visit_right = F;
                    break;

                case EXPR_TYPE_T_AND_EXPR_C:
                    DPRINTF(3, KERN_EMERG "AND_EXPR\n");
                    visit_left = T;
                    visit_right = T;
                    break;

                case EXPR_TYPE_T_NOT_EXPR_C:
                    DPRINTF(3, KERN_EMERG "NOT_EXPR\n");
                    visit_left = T;
                    visit_right = F;
                    break;

                case EXPR_TYPE_T_OR_EXPR_C:
                    DPRINTF(3, KERN_EMERG "OR_EXPR\n");
                    visit_left = T;
                    visit_right = T;
                    break;
    
                case EXPR_TYPE_T_ROOT_EXPR_C:
                    DPRINTF(3, KERN_EMERG "ROOT_EXPR\n");
                    if(expr->left_link == 0) return T; /* no left link */
                    visit_left = T;
                    visit_right = F;
                    break;

                default:
                    /* invalid expression */
                    printk(KERN_EMERG "invalid expression\n");
                    return -1;
            }

            is_leaf = T;
            if(visit_left) {
                DPRINTF(3, KERN_EMERG "shall visit left (%d) %d\n", expr->left_link, v[expr->left_link]);
                if(v[expr->left_link] == -1) {
                    DPRINTF(3, KERN_EMERG "push %d\n", expr->left_link);
                    monitor_push_stack(cond_stack, expr->left_link);
                    is_leaf = F;
                }
            }

            if(visit_right) {
                DPRINTF(3, KERN_EMERG "shall visit right (%d) %d\n", expr->right_link, v[expr->right_link]);
                if(v[expr->right_link] == -1) {
                    DPRINTF(3, KERN_EMERG "push %d\n", expr->right_link);
                    monitor_push_stack(cond_stack, expr->right_link);
                    is_leaf = F;
                }
            }

            if(!is_leaf) {
                DPRINTF(3, KERN_EMERG "not leaf, next.\n");
                goto filter_next;
            }

            DPRINTF(3, KERN_EMERG "looks like a leaf\n");
            /* both children (if any) have been processed by now */
            switch(expr->expr_type) {
                case EXPR_TYPE_T_ROOT_EXPR_C:
                    return v[expr->left_link];
                case EXPR_TYPE_T_AND_EXPR_C:
                    cond_value = v[expr->left_link] & v[expr->right_link];
                    break;
                case EXPR_TYPE_T_NOT_EXPR_C:
                    cond_value = NOT(v[expr->left_link]);
                    break;
                case EXPR_TYPE_T_OR_EXPR_C:
                    cond_value = v[expr->left_link] | v[expr->right_link];
                    break;
                case EXPR_TYPE_T_NO_EXPR_C:
                    cond_value = v[expr->left_link];
                    break;
                default:
                    /* invalid expression */
                    printk(KERN_EMERG "invalid expression\n");
                    return -1;
            }
            DPRINTF(3, KERN_EMERG "expression: cond_value=%d\n", cond_value);
            break;

        case MON_CHECK_TYPE_T_BOT_C:
            bottom = T;
            break;

        case MON_CHECK_TYPE_T_POINT_C:
            printk(KERN_EMERG "MON_CHECK_TYPE_T_POINT_C unimplemented\n");
            return -1;

        default:
            /* invalid check type */
            printk(KERN_EMERG "invalid check type\n");
            return -1;
    }

    DPRINTF(3, KERN_EMERG "cond_value=%d\n", cond_value);
    if(cond_value == -1) {
        offset = m->conditions[cond].comp_rec.msg_off;
        if(!bottom)
            offset += BUFFER_BOTTOM_SIZE;

        switch(m->conditions[cond].comp_rec.check_type & 0x0F) {
            case MON_CHECK_TYPE_T_DW_C:
                value = *(dword*)(skb->data + offset);
                break;
            case MON_CHECK_TYPE_T_W_C:
                value = *(word*)(skb->data + offset);
                break;
            case MON_CHECK_TYPE_T_B_C:
                value = *(byte*)(skb->data + offset);
                break;
            default:
                /* invalid check type */
                printk(KERN_EMERG "invalid check type\n");
                return -1;
        }
  
        cond_value = F;
        switch(m->conditions[cond].comp_rec.comp_type) {
            case MON_COMPARE_T_EQUAL_C:
                if(value == m->conditions[cond].comp_rec.lo_val)
                    cond_value = T;
                break;
            case MON_COMPARE_T_NOT_EQUAL_C:
                if(value != m->conditions[cond].comp_rec.lo_val)
                    cond_value = T;
                break;
            case MON_COMPARE_T_ABOVE_C:
                if(value > m->conditions[cond].comp_rec.lo_val)
                    cond_value = T;
                break;
            case MON_COMPARE_T_ABOVE_EQU_C:
                if(value >= m->conditions[cond].comp_rec.lo_val)
                    cond_value = T;
                break;
            case MON_COMPARE_T_BELOW_C:
                if(value < m->conditions[cond].comp_rec.lo_val)
                    cond_value = T;
                break;
            case MON_COMPARE_T_BELOW_EQU_C:
                if(value <= m->conditions[cond].comp_rec.lo_val)
                    cond_value = T;
                break;
            case MON_COMPARE_T_BETWEEN_C:
                printk(KERN_EMERG "BETWEEN unimplemented\n");
                return -1; /* unimplemented */
                break;
            case MON_COMPARE_T_BETWEEN_EQU_C:
                printk(KERN_EMERG "BETWEEN_EQU unimplemented\n");
                return -1; /* unimplemented */
                break;
            case MON_COMPARE_T_EQU_BIT:
                printk(KERN_EMERG "EQU_BIT unimplemented\n");
                return -1; /* unimplemented */
                break;
            case MON_COMPARE_T_MASK_BIT:
                printk(KERN_EMERG "MASK_BIT unimplemented\n");
                return -1; /* unimplemented */
                break;
            default:
                /* invalid comparison type */
                printk(KERN_EMERG "invalid comparison\n");
                return -1;
        }
        DPRINTF(3, KERN_EMERG "monitored, cond_value=%d\n", cond_value);

        /* false, check for fail branch */
        if(!cond_value && (m->conditions[cond].comp_rec.next_if_ok != 
           m->conditions[cond].comp_rec.next_if_fail)) {
            cond = m->conditions[cond].comp_rec.next_if_fail;
            DPRINTF(3, KERN_EMERG "F, continue to %d\n", cond);
            goto filter_again; /* tail recursion */
        }
    }

    DPRINTF(3, KERN_EMERG "storing cond_value==%d to v[%d]\n", cond_value, cond_store);
    v[cond_store] = cond_value;
    DPRINTF(3, KERN_EMERG "...and pop\n");
    monitor_pop_stack(cond_stack);
    goto filter_next;
}

/* must be called with monitor_table_lock held */
static int monitor_matches(struct sk_buff *skb, int mid, mon_points_t p)
{
    struct dmxmsg_monitor *m;
    int r;
    signed char *value;
    short *cond_stack;

    m = monitor_table[mid].info;
    if(!(m->mon_points & p))
        return F;

    /* allocate memory for values and stack */
    value = kmalloc(MAX_COUNT_OF_MON_RECS + MAX_COUNT_OF_MON_RECS*2, GFP_ATOMIC);
    /* out of memory, better drop the message */
    if(!value)
        return F; 
    /* clear out values */
    memset(value, -1, MAX_COUNT_OF_MON_RECS);

    /* stack starts from the end of allocated memory */
    cond_stack = (short*)(value + MAX_COUNT_OF_MON_RECS + MAX_COUNT_OF_MON_RECS*2);
    /* end marker */
    monitor_push_stack(&cond_stack, -1); 
    monitor_push_stack(&cond_stack, m->first_cond);
  
    DPRINTF(3, KERN_EMERG "calling monitor_filter_sub(%d)\n", m->first_cond);
    r = monitor_filter_sub(m, skb, &cond_stack, value);
    DPRINTF(3, KERN_EMERG "got %d\n", r);
    /* invalid monitor filter */
    if(r == -1) {
        r = F;
    }

    kfree(value); /* releases stack also */
    return r;
}

int dmxmsg_recv_monitor(struct dmxmsg_monitor *mon, struct sk_buff **recv_skb)
{
    int r = 0;

    while((*recv_skb = skb_dequeue(&mon->msg_queue)) == 0) {
        DEFINE_WAIT(wait);
        DPRINTF(3, "dmxmsg_recv_monitor(): going to sleep\n");
        prepare_to_wait(&mon->msg_wait, &wait, TASK_INTERRUPTIBLE);
        if(skb_queue_empty(&mon->msg_queue))
            schedule();
        finish_wait(&mon->msg_wait, &wait);
        DPRINTF(3, "dmxmsg_recv_monitor(): triggered\n");
        if(signal_pending(current)) {
            r = -EINTR;
            goto out;
        }
    }
out:
    return r;
}

/* must be called with monitor_table_lock held */
static void monitor_send(struct sk_buff *skb, int mid)
{
    skb_queue_tail(&monitor_table[mid].info->msg_queue, skb);
    DPRINTF(3, "waking up monitor\n");
    wake_up(&monitor_table[mid].info->msg_wait);
}

static void fill_monitor_header(struct sk_buff *skb, mon_points_t points)
{
    int points_offset;
    int lost_offset;

    do_gettimeofday((struct timeval *)skb->data);

    /* size of the time field depends on the platform */
    if(sizeof(long) == 4) {
        /* 32-bit */
        points_offset = 12;
        lost_offset = 8;
    }
    else if(sizeof(long) == 8) {
        /* 64-bit */
        points_offset = 20;
        lost_offset = 16;
    }

    /* number of lost messages */
    switch(points) {
        case MON_POINTS_T_ARRIVE_C:
            skb->data[points_offset] = 2;
            break;
        case MON_POINTS_T_SEND_C:
            skb->data[points_offset] = 0;
            break;
        case MON_POINTS_T_RECEIVE_C:
            skb->data[points_offset] = 1;
            break;
    }
    *(unsigned int *)&skb->data[lost_offset] = 0;
}

static void _monitor(struct sk_buff *skb, mon_points_t points)
{
    int i;
    struct sk_buff *copy = 0;

    fill_monitor_header(skb, points);
    spin_lock_bh(&monitor_table_lock);

    /* must copy the message to preserve the message time */
    for(i = 0; i < MONITOR_TABLE_SIZE; i++) {
        if(monitor_table[i].active) {
            if(skb_is_nonlinear(skb) &&
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,15)
               skb_linearize(skb)) {
#else
               skb_linearize(skb, GFP_ATOMIC)) {
#endif
                /* memory allocation failed */
                goto out;
            }

            if(monitor_matches(skb, i, points)) {
                if(!copy) {
                    copy = skb_copy(skb, GFP_ATOMIC);
                    monitor_send(copy, i);
                } else {
                    monitor_send(skb_clone(copy, GFP_ATOMIC), i);
                }
            }
        }
    }

out:
    spin_unlock_bh(&monitor_table_lock);
}

static int transmit_msg(struct sk_buff *skb)
{
    struct dmxmsg_client *dest;
    struct dmxmsg_pid dest_pid;
    int r;

    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);
    DPRINTF(3, "transmit_msg(%p)\n", skb);
    get_dest_pid(skb, &dest_pid);
    DPRINTF(3, "getting client\n");

    dest = find_client(&dest_pid);
    if(!dest) {
        /* try to find an error handler pid */
        dest_pid.process_id = 0xFFFF;
        dest = find_client(&dest_pid);
    }
    if(!dest) {
        DPRINTF(1,"No destination for message (%x:%x:%x, number %x)\n",
          h->family, h->process_id, h->focus, h->number);
        if(((buffer_bottom_t*)skb->data)->control & MSG_CONTROL_T_REG_MESSAGE_BIT) {
            send_registered_msg_ack(skb, FAMILY_DOES_NOT_EXIST_EC);
        }
        return -ENXIO;
    }

    DPRINTF(3, "found client (%p)\n", dest);
    if(((buffer_bottom_t*)skb->data)->control & MSG_CONTROL_T_REG_MESSAGE_BIT) {
        send_registered_msg_ack(skb, SUCCESS_EC);
    }

    translate_msg(skb);
    r = store_msg(dest, skb, 0);
    dmxmsg_release_client(dest);
    return r;
}

static void handle_failed_transmit(struct sk_buff *skb, char *msg)
{
    buffer_bottom_t *bb = (buffer_bottom_t*)skb->data;
    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);
    atomic_inc(&dmxmsg_stats.failed_msgs);

    if(h->attributes & MSG_ATTR_T_DONT_CARE_C)
        return;

    printk("%s transmit failed: %s  (Message %s: %04X %04X %04X %02X  "
    "Header: %04X %04X %04X %04X %02X %02X %04X %04X %04X)\n",
    ((bb->control & MSG_CONTROL_T_REG_MESSAGE_BIT)
     ? "Registered message" : "Message"), msg,
    (bb->control & MSG_CONTROL_T_SENDERS_VIEW_C) ? "from" : "to",
    bb->computer,
    bb->family,
    bb->process_id,
    bb->focus,
    h->length,
    h->computer,
    h->family,
    h->process_id,
    h->focus,
    h->attributes,
    h->group,
    h->number,
    h->phys_computer);
}

static void handle_failed_transmit_status(struct sk_buff *skb, int r)
{
    switch(r) {
      case 0:
          handle_failed_transmit(skb, "Success.");
          break;
      case -ENXIO:
          handle_failed_transmit(skb, "No such pid registered.");
          break;
      case -ENOSPC:
          handle_failed_transmit(skb, "Message queue full.");
          break;
      default:
          handle_failed_transmit(skb, "General error.");
          break;
    }
}

static void handle_msg(struct sk_buff *skb, int source)
{
#define ADDR_TABLE_SIZE__C 10
    computer_t addr_table[ADDR_TABLE_SIZE__C];
    int addr_count = ADDR_TABLE_SIZE__C;
    int r;
    int i;
    struct sk_buff *copy;

    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);
    buffer_bottom_t *bb = (buffer_bottom_t*)skb->data;

    DPRINTF(3, "-> %04x:%04x:%04x:%02x\n", h->computer, h->family, h->process_id, h->focus);
  
    /* handle L_ALL_COMP -> PG_ALL/PG_SWT according to trial mode */
    if(bb->next_phys_computer == L_ALL_COMP) {
        if(trial) {
            bb->next_phys_computer = PG_SWT;
        } else {
            bb->next_phys_computer = PG_ALL;
        }
    }

    if((bb->next_phys_computer & COMPUTER_T_INT_C) == COMPUTER_T_INT_PHYS_C) {
        /* the message is for someone on this machine */
        if((source == MSG_SOURCE_POST_OFFICE__C)
         ||(bb->next_phys_computer & COMPUTER_T_MASK_PHYS_MB_C) == dmxmsg_own_phys_addr) {
            r = transmit_msg(skb);
            if(r) {
                handle_failed_transmit_status(skb, r);
                dmxmsg_free_msg(&skb);
            }
            return;
        } else if((((bb->next_phys_computer & COMPUTER_T_MASK_PHYS_MB_C) == PG_ALL) && !trial)
               ||(((bb->next_phys_computer & COMPUTER_T_MASK_PHYS_MB_C) == PG_SWT) && trial)) {
            /* broadcast, must make a copy for the local user,
               message data will be modified as the buffer is translated */

            copy = skb_copy(skb, GFP_ATOMIC);
            if(!copy) {
                handle_failed_transmit(skb, "skb_copy() failed.");
            } else {
                r = transmit_msg(copy);
                if(r) {
                    handle_failed_transmit_status(skb, r);
                    dmxmsg_free_msg(&copy);
                }
            }
        }

        /* going outside */
        DPRINTF(3, "to post office\n");
        if(source != MSG_SOURCE_POST_OFFICE__C) {
            r = send_to_post_office(skb, &bb->next_phys_computer, 1);
            if(r) {
                handle_failed_transmit(skb, "post_office->send() failed.");
                dmxmsg_free_msg(&skb);
            }
        } else {
            dmxmsg_free_msg(&skb);
        }
    } else if((bb->next_phys_computer & COMPUTER_T_INT_C) == COMPUTER_T_INT_LOG_C) {
        /* logical address */
        if(dmxmsg_own_log_addr == COMPUTER_T_LOG_INVALID_C) {
              handle_failed_transmit(skb, "Logical addressing unavailable.");
              dmxmsg_free_msg(&skb);
              return;
        }

        r = dmxmsg_logaddr_to_phys(bb->next_phys_computer, addr_table, &addr_count);
        if(r) {
            handle_failed_transmit(skb, "Invalid logical address.");
            dmxmsg_free_msg(&skb);
            return;
        }

        for(i = 0; i < addr_count; i++) {
            /* own unit's address was found in the destination addresses */
            if((addr_table[i] == dmxmsg_own_phys_addr) || (addr_table[i] == R_OWN_UNIT)) {
                /* remove from address list */
                addr_table[i] = addr_table[addr_count - 1];
                addr_count--;
                i--;
        
                copy = skb_copy(skb, GFP_ATOMIC);
                if(!copy) {
                    handle_failed_transmit(skb, "skb_copy() failed.");
                } else {
                    r = transmit_msg(copy);
                    if(r) {
                        handle_failed_transmit_status(skb, r);
                        dmxmsg_free_msg(&copy);
                    }
                }
            }
        }

        /* going outside */
        DPRINTF(3, "to post office (logical)\n");
        if((source != MSG_SOURCE_POST_OFFICE__C) && (addr_count > 0)) {
            r = send_to_post_office(skb, addr_table, addr_count);
            if(r) {
                handle_failed_transmit(skb, "post_office->send() failed.");
                dmxmsg_free_msg(&skb);
            }
        } else {
            dmxmsg_free_msg(&skb);
        }
    } else if(bb->next_phys_computer == COMPUTER_T_REL_OWN_UNIT_C) {
        r = transmit_msg(skb);
        if(r) {
            dmxmsg_free_msg(&skb);
        }
    } else if(bb->next_phys_computer == COMPUTER_T_REL_CO_UNIT_C) {
        if(co_unit_address == P_NOT_DEFINED) {
            handle_failed_transmit(skb, "Co-unit unavailable.");
            dmxmsg_free_msg(&skb);
            return;
        }
        bb->next_phys_computer = co_unit_address;
        handle_msg(skb, source);
    }
}

/* TBD: combine dmxmsg_send_msg and dmxmsg_send_msg_as */
void dmxmsg_send_msg(struct dmxmsg_client *client, struct sk_buff **send_skb)
{
    struct sk_buff *skb = *send_skb;
    msg_header_t *msg = (msg_header_t*)skb->data;

    *send_skb = 0;
    DPRINTF(3, "dmxmsg_send_msg()\n");

    if(msg->length < sizeof(msg_header_t)) {
        printk("Dropped invalid message: invalid length\n");
        dmxmsg_free_msg(&skb);
        return;
    }

    if((msg->attributes & MSG_ATTR_T_DESTROY_IF_NOT_WO_C)
     &&((msg->computer & COMPUTER_T_INT_C) == COMPUTER_T_INT_LOG_C)
    &&!((own_unit_state | UNIT_STATE_T_MAINSTATE_BM_C) == UNIT_STATE_T_WO_C)) {
        DPRINTF(3, "dropped message: not in WO state\n");
        dmxmsg_free_msg(&skb);
        return;
    }

    DPRINTF(3, "bufbot\n");
    fill_buffer_bottom(client, skb);
    DPRINTF(3, "monitor\n");
    monitor(skb, MON_POINTS_T_SEND_C);
    DPRINTF(3, "handle\n");
    handle_msg(skb, MSG_SOURCE_LOCAL__C);
}

void dmxmsg_send_msg_as(struct dmxmsg_client *client,
  struct sk_buff **send_skb, computer_t from_comp, family_t from_fam,
  process_id_t from_proc, focus_t from_foc)
{
    struct sk_buff *skb = *send_skb;
    msg_header_t *msg = (msg_header_t*)skb->data;
    buffer_bottom_t *bb;

    *send_skb = 0;
    if(msg->length < sizeof(msg_header_t)) {
        printk("Dropped invalid message: invalid length\n");
        dmxmsg_free_msg(&skb);
        return;
    }

    if((msg->attributes & MSG_ATTR_T_DESTROY_IF_NOT_WO_C)
       && !((own_unit_state | UNIT_STATE_T_MAINSTATE_BM_C) == UNIT_STATE_T_WO_C)) {
      DPRINTF(3, "dropped message: not in WO state\n");
        dmxmsg_free_msg(&skb);
        return;
    }

    fill_buffer_bottom(client, skb);
    bb = (buffer_bottom_t*)skb->data;
    if(from_comp != COMPUTER_T_ANY_C)
        bb->computer = from_comp;
    bb->family = from_fam;
    bb->process_id = from_proc;
    bb->focus = from_foc;
    monitor(skb, MON_POINTS_T_SEND_C);
    handle_msg(skb, MSG_SOURCE_LOCAL__C);
}

/* dequeue a message from the highest priority message queue, or 0 if
 * all queues are empty */
static inline struct sk_buff *client_dequeue(struct dmxmsg_client *client)
{
    int i;
    struct sk_buff *s = 0;

    for(i = MSG_PRIORITY_COUNT__C - 1; i >= 0; i--) {
        s = skb_dequeue(&client->msg_queue[i]);
        if(s)
            break;
    }
    return s;
}

/* check if a client's message queues of all priorities are empty */
static inline int client_queue_empty(struct dmxmsg_client *client)
{
    int i;

    for(i = 0; i < MSG_PRIORITY_COUNT__C; i++) {
        if(!skb_queue_empty(&client->msg_queue[i]))
            return 0;
    }
    return 1;
}

int dmxmsg_recv_whole_msg(struct dmxmsg_client *client,
        struct sk_buff **recv_skb, int nonblock)
{
    int r = 0;

    dmxmsg_hold_client(client);
    while(1) {
        DEFINE_WAIT(wait);
        if((*recv_skb = client_dequeue(client)))
            break;

        if(nonblock) {
            DPRINTF(3, "dmxmsg_recv_whole_msg(): nonblock, don't sleep\n");
            r = -EAGAIN;
            goto out;
        }

        DPRINTF(3, "dmxmsg_recv_whole_msg(): going to sleep\n");
        prepare_to_wait(&client->msg_wait, &wait, TASK_INTERRUPTIBLE);
        if(client_queue_empty(client))
            schedule();
        finish_wait(&client->msg_wait, &wait);
        DPRINTF(3, "dmxmsg_recv_whole_msg(): triggered\n");

        if(signal_pending(current)) {
            r = -EINTR;
            goto out;
        }
    }

    DPRINTF(3, "dmxmsg_recv_whole_msg(): got message\n");
    spin_lock_bh(&client->lock);
    client->msg_queue_size -= (*recv_skb)->len;
    spin_unlock_bh(&client->lock);
    atomic_sub((*recv_skb)->len, &dmxmsg_stats.memory_in_use);
    atomic_dec(&dmxmsg_stats.buffers_in_use);

    r = 0;
    monitor(*recv_skb, MON_POINTS_T_RECEIVE_C);
out:
    dmxmsg_release_client(client);
    return r;
}

int dmxmsg_recv_msg(struct dmxmsg_client *client, struct sk_buff **recv_skb,
  int nonblock)
{
    int r = dmxmsg_recv_whole_msg(client, recv_skb, nonblock);
    if(r)
        return r;

    /* strip buffer_bottom_t */
    skb_pull(*recv_skb, BUFFER_BOTTOM_SIZE);
    DPRINTF(3, "dmxmsg_recv_msg(): got message\n");
    return 0;
}

int dmxmsg_return_msg(struct dmxmsg_client *client, struct sk_buff **return_skb)
{
    int r;
    struct sk_buff *skb = *return_skb;

    dmxmsg_hold_client(client);
    r = store_msg(client, skb, 1);
    dmxmsg_release_client(client);

    if(!r) {
        /* cannot use message after returning it */
        *return_skb = 0;
    }

    return r;
}

int dmxmsg_recv_from_post_office(struct sk_buff *skb)
{
    buffer_bottom_t *bb = (buffer_bottom_t*)(skb->data);
    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);

    if(debugflag)
    printk(KERN_EMERG "%04x from %04x:%04x:%04x:%02x to %04x:%04x:%04x:%02x (%04x)\n",
      h->number, bb->computer, bb->family, bb->process_id, bb->focus,
      h->computer, h->family, h->process_id, h->focus, bb->next_phys_computer);

    if(separate) {
        /* these are the messages that should not be ignored */
        if(!((h->number == GET_COMPUTER_STATUS_SN)
          ||(h->number == NORMAL_RESET_COMMAND_SN)
          ||(h->number == TOTAL_RESET_COMMAND_SN))) {
            kfree_skb(skb);
            return 0;
        }
    }

    if((((bb->next_phys_computer & COMPUTER_T_MASK_PHYS_MB_C) == PG_ALL) && trial)
      ||(((bb->next_phys_computer & COMPUTER_T_MASK_PHYS_MB_C) == PG_SWT) && !trial)) {
        /* the message was sent to different broadcast group */
        kfree_skb(skb);
        return 0;
    }

    handle_msg(skb, MSG_SOURCE_POST_OFFICE__C);
    return 0;
}

static int send_to_post_office(struct sk_buff *skb, computer_t *dst, int dst_count)
{
    int r = 0;
    msg_header_t *h = (msg_header_t*)(skb->data + BUFFER_BOTTOM_SIZE);

    if(separate) {
        /* these are the messages that should not be ignored */
        if(!((h->number == GET_COMPUTER_STATUS_ACK_SN)
             || (h->number == SEPARATE_COMMAND_ACK_SN)
             || (h->number == RESET_COMMAND_ACK_SN))) {
            kfree_skb(skb);
            return 0;
        }
    }

    if(current_post_office && current_post_office->send) {
        r = current_post_office->send(skb, dst, dst_count);
    } else {
        kfree_skb(skb);
    }
    return r;
}

int dmxmsg_register_post_office(struct dmxmsg_post_office *po)
{
    DPRINTF(1, "dmxmsg_register_post_office()\n");

    if(current_post_office) {
        printk("dmxmsg_core: Post office already registered\n");
        return -EBUSY;
    }
    current_post_office = po;

    return 0;
}

int dmxmsg_unregister_post_office(void)
{
    DPRINTF(1, "dmxmsg_unregister_post_office()\n");
    current_post_office = 0;
    return 0;
}

static int dmxmsg_parse_args(void)
{
  DPRINTF(1, "physaddr = %x\n", physaddr);

    if(!((physaddr >= 0) && (physaddr <= 0x3ff))
      ||(physaddr == PG_SYM) || (physaddr == PG_ALL) || (physaddr == PG_SWT)) {
        DPRINTF(1, "Invalid physical address 0x%x\n", physaddr);
        return -EINVAL;
    }

    dmxmsg_own_phys_addr = physaddr;
    return 0;
}

static void reset_dmxmsg_statistics(void)
{
    DPRINTF(1, "reset_dmxmsg_statistics()\n");
    memset( &dmxmsg_stats, 0x00, sizeof(dmxmsg_stats));
    spin_lock_init(&dmxmsg_stats.peak_lock);
}

static int __init dmxmsg_init(void)
{
    int err = 0;

    memset(client_table, 0, CLIENT_TABLE_SIZE * sizeof(struct dmxmsg_client*));
    client_table_used_count = 0;

    DPRINTF(1, "dmxmsg_init()\n");
    err = dmxmsg_parse_args();
    if(err) {
        return err;
    }

    reset_dmxmsg_statistics();
    err = dmxmsg_proc_create();
    if(err) {
        return err;
    }

    spin_lock_init(&client_table_lock);
    spin_lock_init(&logaddr_table_lock);
    spin_lock_init(&free_process_id_lock);
    spin_lock_init(&pid_registration_lock);
    spin_lock_init(&msg_seq_number_lock);
    INIT_LIST_HEAD(&client_list);
    init_monitor_table();
    return err;
}


static void __exit dmxmsg_exit (void)
{
    DPRINTF(1, "dmxmsg_exit()\n");
    dmxmsg_proc_delete();
}

module_init(dmxmsg_init);
module_exit(dmxmsg_exit);

MODULE_AUTHOR("Lauri Koponen");
//MODULE_LICENSE("GPL");

EXPORT_SYMBOL(dmxmsg_is_unit_separated);
EXPORT_SYMBOL(dmxmsg_own_phys_addr);
EXPORT_SYMBOL(dmxmsg_alloc_client);
EXPORT_SYMBOL(dmxmsg_release_client);
EXPORT_SYMBOL(dmxmsg_set_queue_size);
EXPORT_SYMBOL(dmxmsg_register_client);
EXPORT_SYMBOL(dmxmsg_register_pid);
EXPORT_SYMBOL(dmxmsg_unregister_all_pids);
EXPORT_SYMBOL(dmxmsg_force_unregister_pid);
EXPORT_SYMBOL(dmxmsg_get_active_pid);
EXPORT_SYMBOL(dmxmsg_set_active_pid);
EXPORT_SYMBOL(dmxmsg_alloc_msg);
EXPORT_SYMBOL(dmxmsg_resize_msg);
EXPORT_SYMBOL(dmxmsg_free_msg);
EXPORT_SYMBOL(dmxmsg_send_msg);
EXPORT_SYMBOL(dmxmsg_send_msg_as);
EXPORT_SYMBOL(dmxmsg_recv_whole_msg);
EXPORT_SYMBOL(dmxmsg_recv_msg);
EXPORT_SYMBOL(dmxmsg_return_msg);
EXPORT_SYMBOL(dmxmsg_recv_from_post_office);
EXPORT_SYMBOL(dmxmsg_register_post_office);
EXPORT_SYMBOL(dmxmsg_unregister_post_office);
EXPORT_SYMBOL(dmxmsg_alloc_monitor);
EXPORT_SYMBOL(dmxmsg_release_monitor);
EXPORT_SYMBOL(dmxmsg_register_monitor);
EXPORT_SYMBOL(dmxmsg_unregister_monitor);
EXPORT_SYMBOL(dmxmsg_recv_monitor);

