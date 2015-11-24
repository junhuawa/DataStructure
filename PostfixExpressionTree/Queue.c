#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "Queue.h"

void Enqueue(Tree X, Queue Q);
Tree Front(Queue Q);
void Dequeue(Queue Q);
Tree FrontAndDequeue(Queue Q);
void DisposeQueue(Queue D);

Queue CreateQueue()
{
    Queue q = NULL;

    q = malloc(sizeof(struct QueueRecord));
    if(!q) {
        return NULL;
    }

    q->Front = malloc(sizeof(struct Node));
    if(!q->Front) {
        free(q);
        return NULL;
    }
    q->Front->Next = NULL;
    q->Front->Last = NULL;

    q->Rear = q->Front;
    return q;
}

bool IsEmptyQueue(Queue Q)
{
    if(!Q) exit(-1);
    return (Q->Front == Q->Rear);
}

void DisposeQueue(Queue Q)
{
    if(!Q) return;

    while(!IsEmptyQueue(Q)) {
        Dequeue(Q);
    }
    free(Q->Front);
    free(Q);
}


void Enqueue( Tree X, Queue Q)
{
    PtrToNode ptr = NULL;

    if(!Q) exit(-1);

    ptr = malloc(sizeof(struct Node));
    if(!ptr) exit(-1); 
    ptr->Element = X;
    ptr->Next = NULL;

    if(IsEmptyQueue(Q)) {
        Q->Front->Next = ptr;
        ptr->Last = NULL;
    }else {
        Q->Rear->Next = ptr;
        ptr->Last = Q->Rear;
    }

    Q->Rear = ptr;
}

bool IsOnlyOneElementLeft(PtrToNode ptr, Queue Q)
{
    return ((Q->Rear == ptr)&&(Q->Front->Next == ptr));
}

Tree FrontAndDequeue(Queue Q)
{
    PtrToNode ptr = NULL;
    Tree value;

    if(!Q) exit(-1);
    if(IsEmptyQueue(Q)) return NULL;

    ptr = Q->Front->Next;
    if(IsOnlyOneElementLeft(ptr, Q)) {
        Q->Rear = Q->Front;
    }else {
        (ptr->Next)->Last = NULL;
    }
    Q->Front->Next = ptr->Next;

    value = ptr->Element;
    free(ptr);
    return value;
}

void Dequeue(Queue Q)
{
    PtrToNode ptr = NULL;

    if(!Q) exit(-1);
    if(IsEmptyQueue(Q))return; 

    ptr = Q->Front->Next;
    if(IsOnlyOneElementLeft(ptr, Q)) {
        Q->Rear = Q->Front;
    }else {
        (ptr->Next)->Last = NULL;
    }
    Q->Front->Next = ptr->Next;

    free(ptr);
    return;
}

#ifdef __UT_QUEUE__

typedef void test(void);

void PrintInOrder(Queue Q)
{
    PtrToNode ptr = Q->Front->Next;
    while(ptr != NULL) {
        printf("%p", ptr->Element);
        if(ptr->Next) {
            printf(" ");
        }
        ptr = ptr->Next;
    }
   printf("\n");
}
//
//   void PrintInReverseOrder(Deque D)
//   {
//       PtrToNode ptr = D->Rear;
//       while(ptr != (PtrToNode)D) {
//           printf("%d", ptr->Element);
//           if(ptr->Last) {
//               printf(" ");
//           }
//           ptr = ptr->Last;
//       }
//      printf("\n");
//
//   } 
//
void Print(Queue Q) {
    PrintInOrder(Q);
    //PrintInReverseOrder(D);
}
void Queue_is_Empty()
{
    Queue Q;
    Q = CreateQueue();
    assert(IsEmptyQueue(Q) == true); 
    DisposeQueue(Q);
}
//
void Queue_is_not_Empty()
{
    Queue Q;
    Tree tree[10] = {(Tree )0x1, (Tree )0x2, (Tree )0x3, (Tree )0x4, (Tree )0x5, (Tree )0x6, (Tree )0x7, (Tree )0x8, (Tree )0x9, (Tree )0xA };
    Tree tmp;

    Q = CreateQueue();
    Enqueue(tree[0], Q);
    assert(IsEmptyQueue(Q) == false); 
    //Print(Q);
    tmp = FrontAndDequeue(Q);
    assert(tmp == tree[0]);
    DisposeQueue(Q);
}

void Queue_is_correct_for_En_and_Dequeue()
{
    Queue Q;
    int i;
    Tree tree[10] = {(Tree )0x1, (Tree )0x2, (Tree )0x3, (Tree )0x4, (Tree )0x5, (Tree )0x6, (Tree )0x7, (Tree )0x8, (Tree )0x9, (Tree )0xA };
    Tree tmp;

    Q = CreateQueue();
    for(i=0; i<10; i++) {
        Enqueue(tree[i], Q);
    }

    assert(IsEmptyQueue(Q) == false); 
    //Print(Q);
    for(i=0; i<10; i++) {
        tmp = FrontAndDequeue(Q);
        assert(tmp == tree[i]);
    }
    assert(IsEmptyQueue(Q) == true); 
    DisposeQueue(Q);
}

static test *tests[] =
{
    Queue_is_Empty,
    Queue_is_not_Empty,
    Queue_is_correct_for_En_and_Dequeue,
    (test *)0,
};

int main(void)
{
    size_t at = 0;
    while(tests[at])
    {
        tests[at++]();
    }
    printf("%ld tests passed\n", at);
    return 0;    
}
#endif
