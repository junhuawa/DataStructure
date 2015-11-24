#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "Deque.h"

void Initialize(Deque D);

Deque CreateDeque()
{
    Deque dq = NULL;

    dq = malloc(sizeof(struct DequeRecord));
    if(!dq) {
        return NULL;
    }
    Initialize(dq);
    //printf("dq: %p\n", dq);
    return dq;
}

bool IsEmpty(Deque D)
{
    return (D->Front == (PtrToNode)D) &&(D->Rear == (PtrToNode)D);
}

void DisposeDeque(Deque D)
{
    if(!D) return;

    while(!IsEmpty(D)) {
        Pop(D);
    }
    free(D);
}

void Initialize(Deque D)
{
    D->Front = D->Rear = (PtrToNode) D;
}

int Push( ElementType X, Deque D )
{
    PtrToNode ptr = NULL;
    PtrToNode tmp = NULL;

    if(!D) return false;

    //printf("D in Push: %p\n", D);
    ptr = malloc(sizeof(struct Node));
    if(!ptr) return false;
    ptr->Element = X;
    ptr->Last = (PtrToNode)D;

    if(IsEmpty(D)) {
        ptr->Next = (PtrToNode)D;
        D->Front = ptr;
        D->Rear = ptr;
        return true;
    }

    tmp = D->Front;
    ptr->Next = tmp;
    tmp->Last = ptr;
    D->Front = ptr;
    return true;
}

bool IsLastElement(PtrToNode ptr, Deque D)
{
    return (ptr->Next == (PtrToNode)D);
}

ElementType Pop( Deque D )
{
    PtrToNode ptr = NULL;
    ElementType value;

    if(!D) return ERROR;

    if(IsEmpty(D)) return ERROR;

    ptr = D->Front;

    if(IsLastElement(ptr, D)) {
        Initialize(D);
    }else {
        D->Front = ptr->Next;
        (ptr->Next)->Last = (PtrToNode)D;
    }

    value = ptr->Element;
    free(ptr);
    return value;
}

int Inject( ElementType X, Deque D )
{
    PtrToNode ptr = NULL;
    PtrToNode tmp = NULL;

    if(!D) return false;

    ptr = malloc(sizeof(struct Node));
    if(!ptr) return false;
    ptr->Element = X;
    ptr->Next = (PtrToNode)D;

    if(IsEmpty(D)) {
        ptr->Last = (PtrToNode)D;
        D->Front = ptr;
        D->Rear = ptr;
        return true;
    }

    tmp = D->Rear;
    tmp->Next = ptr;
    ptr->Last = tmp;
    D->Rear = ptr;
    return true;
}

bool IsFirstElement(PtrToNode ptr, Deque D)
{
    return (ptr->Last == (PtrToNode)D);
}

ElementType Eject( Deque D )
{
    PtrToNode ptr = NULL;
    ElementType value;

    if(!D) return ERROR;

    if(IsEmpty(D)) return ERROR;

    ptr = D->Rear;

    if(IsFirstElement(ptr, D)) {
        Initialize(D);
    }else {
        D->Rear = ptr->Last;
        (ptr->Last)->Next = (PtrToNode)D;
    }

    value = ptr->Element;
    free(ptr);
    return value;
}


#ifdef __UT__

typedef void test(void);

void PrintInOrder(Deque D)
{
    PtrToNode ptr = D->Front;
    while(ptr != (PtrToNode)D) {
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
    PtrToNode ptr = D->Rear;
    while(ptr != (PtrToNode)D) {
        printf("%d", ptr->Element);
        if(ptr->Last) {
            printf(" ");
        }
        ptr = ptr->Last;
    }
   printf("\n");

} 

void Print(Deque D) {
    PrintInOrder(D);
    PrintInReverseOrder(D);
}
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
    //Print(D);
    assert(Pop(D) == 1);
    rc = Inject(10, D);
    //Print(D);
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

    //Print(D);
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
    //Print(D);
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
    assert(Eject(D) == 2);
    assert(Eject(D) == 1);
    assert(Eject(D) == ERROR);
    //Print(D);

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
    //Print(D);
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
