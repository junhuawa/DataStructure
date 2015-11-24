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
