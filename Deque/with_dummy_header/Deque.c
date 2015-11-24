#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "Deque.h"

struct Node;
typedef struct Node *PtrToNode;
struct Node {
    ElementType Element;
    PtrToNode Next, Last;
};

struct DequeRecord {
    PtrToNode Front, Rear;
};

PtrToNode header = NULL;

Deque CreateDeque()
{
    Deque dq = NULL;
    PtrToNode ptr = NULL;

    dq = malloc(sizeof(struct DequeRecord));
    if(!dq) {
        return NULL;
    }
    ptr = malloc(sizeof(struct Node));
    if(!ptr) {
        free(dq);
        return NULL;
    }
    ptr->Next = NULL;
    ptr->Last = NULL;

    dq->Front = dq->Rear = ptr;
    header = ptr;
    return dq;
}

bool IsEmpty(Deque D)
{
    return ((D->Front == D->Rear) &&(D->Rear == header));
}

void DisposeDeque(Deque D)
{
    if(!D) return;

    while(!IsEmpty(D)) {
        Pop(D);
    }
    free(D->Front);
    free(D);
}

int Push( ElementType X, Deque D )
{
    PtrToNode ptr = NULL;

    if(!D) return false;

    ptr = malloc(sizeof(struct Node));
    if(!ptr) return false;
    ptr->Element = X;

    ptr->Next = header->Next;
    ptr->Last = NULL;
    if(IsEmpty(D)) {
        D->Rear = ptr;
    }else {
        (header->Next)->Last = ptr;
    }
    header->Next = ptr; 
    return true;
}

bool IsOnlyOneElementLeft(PtrToNode ptr, Deque D)
{
    return ((D->Rear == ptr)&&(D->Front->Next == ptr));
}

ElementType Pop( Deque D )
{
    PtrToNode ptr = NULL;
    ElementType value;

    if(!D) return ERROR;

    if(IsEmpty(D)) return ERROR;

    ptr = header->Next;

    if(IsOnlyOneElementLeft(ptr, D)) {
        D->Rear = header;
    }else {
        (ptr->Next)->Last = NULL;
    }
    header->Next = ptr->Next;

    value = ptr->Element;
    free(ptr);
    return value;
}

int Inject( ElementType X, Deque D )
{
    PtrToNode ptr = NULL;
    PtrToNode tail = D->Rear;

    if(!D) return false;

    ptr = malloc(sizeof(struct Node));
    if(!ptr) return false;
    ptr->Element = X;

    if(IsEmpty(D)) {
        ptr->Last = NULL;
        header->Next = ptr;
    }else {
        ptr->Last = tail;
        tail->Next = ptr;
    }
    ptr->Next = NULL;

    D->Rear = ptr;
    return true;
}

ElementType Eject( Deque D )
{
    PtrToNode ptr = NULL;
    ElementType value;

    if(!D) return ERROR;

    if(IsEmpty(D)) return ERROR;

    ptr = D->Rear;

    if(IsOnlyOneElementLeft(ptr, D)) {
        D->Rear = header;
        header->Next = header->Last = NULL;
    }else {
        D->Rear = (D->Rear)->Last;
        (D->Rear)->Next = NULL;
    }

    value = ptr->Element;
    free(ptr);
    return value;
}

void PrintInOrder(Deque D)
{
    PtrToNode ptr = D->Front->Next;
    while(ptr) {
        printf("%d", ptr->Element);
        if(ptr->Next) {
            printf(" ");
        }
        ptr = ptr->Next;
    }
    printf("\n");
}

void PrintInReverseOrder(Deque D)
{
    PtrToNode ptr;
    ptr = D->Rear;
    if(ptr == header) {
        printf("\n");
        return;
    }
    while(ptr) {
        printf("%d", ptr->Element);
        if(ptr->Last) {
            printf(" ");
        }
        ptr = ptr->Last;
    }
    printf("\n");
}

void Print(Deque D)
{
    PrintInOrder(D);
    PrintInReverseOrder(D);
}
#ifdef __UT__

typedef void test(void);
void Deque_is_Empty()
{
    Deque D;
    D = CreateDeque();
    assert(IsEmpty(D) == true); 
    assert(Pop(D) == ERROR);
    assert(Eject(D) == ERROR);
    DisposeDeque(D);
}

void Deque_is_not_Empty()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Push(1, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 
    assert(Pop(D) == 1);
    rc = Inject(10, D);
    assert(rc == true);
    assert(Pop(D) == 10);
    DisposeDeque(D);
}

void Deque_Pop_Correct_Value()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Push(1, D);
    rc = Push(2, D);
    rc = Push(3, D);
    rc = Push(4, D);
    rc = Push(5, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 
    //Print(D);

    assert(Pop(D) == 5);
    assert(Pop(D) == 4);
    assert(Pop(D) == 3);
    assert(Pop(D) == 2);
    assert(Pop(D) == 1);
    assert(Pop(D) == ERROR);
    

    DisposeDeque(D);
}

void Deque_Inject_Correct()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Inject(1, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 
    DisposeDeque(D);
}

void Deque_Eject_Correct_Value()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Inject(1, D);
    rc = Inject(2, D);
    rc = Inject(3, D);
    rc = Inject(4, D);
    rc = Inject(5, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 

    //Print(D);
    assert(Eject(D) == 5);
    assert(Eject(D) == 4);
    assert(Eject(D) == 3);
    //Print(D);
    assert(Eject(D) == 2);
    //Print(D);
    assert(Eject(D) == 1);
    assert(Eject(D) == ERROR);

    DisposeDeque(D);
}

void Deque_Eject_Correct_When_Empty()
{
    Deque D;

    D = CreateDeque();
    assert(IsEmpty(D) == true); 

    assert(Eject(D) == ERROR);

    DisposeDeque(D);
}

void Deque_4_Action_Correct()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Inject(1, D);
    rc = Push(2, D);
    rc = Inject(3, D);
    rc = Push(4, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 
    //Print(D);

    assert(Eject(D) == 3);
    assert(Eject(D) == 1);
    //printf("%d, %d\n", Eject(D), Eject(D));
    assert(Eject(D) == 2);
    assert(Eject(D) == 4);
    assert(Eject(D) == ERROR);

    DisposeDeque(D);
}

void Deque_4_Action_Correct_Pop()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Inject(1, D);
    rc = Push(2, D);
    rc = Inject(3, D);
    rc = Push(4, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 
    //Print(D);

    assert(Pop(D) == 4);
    assert(Pop(D) == 2);
    //Print(D);
    assert(Eject(D) == 3);
    assert(Eject(D) == 1);
    assert(Pop(D) == ERROR);
    

    DisposeDeque(D);
}

void Deque_4_Action_Correct_Pop_1()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Inject(1, D);
    rc = Push(2, D);
    rc = Inject(3, D);
    rc = Push(4, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 
    //PrintDeque(D);

    assert(Pop(D) == 4);
    assert(Pop(D) == 2);
    assert(Pop(D) == 1);
    assert(Pop(D) == 3);
    assert(Pop(D) == ERROR);

    DisposeDeque(D);
}

void Deque_Push_Pop_Push_Eject()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Inject(1, D);
    rc = Push(2, D);
    rc = Inject(3, D);
    rc = Push(4, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 

    assert(Pop(D) == 4);
    assert(Pop(D) == 2);
    assert(Pop(D) == 1);
    assert(Pop(D) == 3);
    assert(Pop(D) == ERROR);
    rc = Inject(10, D);
    rc = Push(20, D);
    rc = Inject(30, D);
    rc = Push(40, D);
    assert(Eject(D) == 30);
    assert(Eject(D) == 10);
    assert(Eject(D) == 20);
    assert(Eject(D) == 40);
    assert(Eject(D) == ERROR);

    DisposeDeque(D);
}

void Deque_Push_Pop_Push_Pop()
{
    Deque D;
    int rc;

    D = CreateDeque();
    rc = Inject(1, D);
    rc = Push(2, D);
    rc = Inject(3, D);
    rc = Push(4, D);
    assert(IsEmpty(D) == false); 
    assert(rc == true); 

    assert(Pop(D) == 4);
    assert(Pop(D) == 2);
    assert(Pop(D) == 1);
    assert(Pop(D) == 3);
    assert(Pop(D) == ERROR);
    rc = Inject(10, D);
    rc = Push(20, D);
    rc = Inject(30, D);
    rc = Push(40, D);
    assert(Pop(D) == 40);
    assert(Pop(D) == 20);
    assert(Pop(D) == 10);
    assert(Pop(D) == 30);
    assert(Pop(D) == ERROR);

    DisposeDeque(D);
}

static test *tests[] =
{
    Deque_is_Empty,
    Deque_is_not_Empty,
    Deque_Pop_Correct_Value,
    Deque_Inject_Correct,
    Deque_Eject_Correct_Value,
    Deque_Eject_Correct_When_Empty,
    Deque_4_Action_Correct,
    Deque_4_Action_Correct_Pop,
    Deque_4_Action_Correct_Pop_1,
    Deque_Push_Pop_Push_Eject,
    Deque_Push_Pop_Push_Pop,
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
