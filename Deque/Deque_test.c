#include <stdio.h>
#include <stdlib.h>

#define ElementType int
#define ERROR 1e5
typedef enum { push, pop, inject, eject, end } Operation;

typedef struct Node *PtrToNode;
struct Node {
    ElementType Element;
    PtrToNode Next, Last;
};
typedef struct DequeRecord *Deque;
struct DequeRecord {
    PtrToNode Front, Rear;
};
Deque CreateDeque();
int Push( ElementType X, Deque D );
ElementType Pop( Deque D );
int Inject( ElementType X, Deque D );
ElementType Eject( Deque D );
Operation GetOp();          /* details omitted */
void PrintDeque( Deque D ); /* details omitted */

Operation GetOp()
{
    char buf[100];
    scanf("%s", buf);
    if(strncmp("Push", buf, 5) == 0) {
        return push;
    }
    if(strncmp("Pop", buf, 4) == 0) {
        return pop;
    }
    if(strncmp("Eject", buf, 6) == 0) {
        return eject;
    }
    if(strncmp("Inject", buf, 7) == 0) {
        return inject;
    }
    if(strncmp("End", buf, 4) == 0) {
        return end;
    }
    return end;
}

void PrintDeque( Deque D)
{
    ElementType value;
    printf("Inside Deque: ");
    value = Pop(D);
    while(value != ERROR) {
        printf("%d", value);
        value = Pop(D);
        if(value != ERROR) {
            printf(" ");
        }
    }
    printf("\n");
    return;
}

int main()
{
    ElementType X;
    Deque D;
    int done = 0;

    D = CreateDeque();
    while (!done) {
        switch(GetOp()) {
        case push: 
            scanf("%d", &X);
            if (!Push(X, D)) printf("Memory is Full!\n");
            break;
        case pop:
            X = Pop(D);
            if ( X==ERROR ) {
             printf("Deque is Empty!\n");   
            }else {
             printf("%d\n", X);   
            }
            break;
        case inject: 
            scanf("%d", &X);
            if (!Inject(X, D)) printf("Memory is Full!\n");
            break;
        case eject:
            X = Eject(D);
            if ( X==ERROR ) printf("Deque is Empty!\n");
            break;
        case end:
            PrintDeque(D);
            done = 1;
            break;
        }
    }
    return 0;
}

#include <string.h>
#include <stdbool.h>
void Initialize(Deque D);

Deque CreateDeque()
{
    Deque dq = NULL;

    dq = malloc(sizeof(struct DequeRecord));
    if(!dq) {
        return NULL;
    }
    Initialize(dq);
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
