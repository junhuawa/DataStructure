#include <stdio.h>
#include <stdlib.h>

typedef int ElementType;
typedef struct Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;
struct Node 
{
    ElementType Element;
    Position Next;
};

List Read(); /* details omitted */
void Print( List L ); /* details omitted */
List Reverse( List L );

int main()
{
    List L1, L2;
    L1 = Read();
    L2 = Reverse(L1);
    Print(L1);
    Print(L2);
    return 0;
}
List CreateList(int num)
{
    PtrToNode head = NULL; 
    PtrToNode ptr = NULL; 
    PtrToNode tmpPtr = NULL; 
    ElementType value;

    head = (PtrToNode)malloc(sizeof(struct Node));
    if(!head) {
        printf("malloc failed\n");
        exit(-1);
    }
    head->Element = -1;
    head->Next = NULL;

    ptr = head;

    while(num) {
        scanf("%d", &value);
        tmpPtr = (PtrToNode)malloc(sizeof(struct Node));
        if(!tmpPtr) {
            printf("malloc failed\n");
            exit(-1);
        }
        tmpPtr->Element = value;
        tmpPtr->Next = NULL;
        ptr->Next = tmpPtr;
        ptr = ptr->Next;
        num --;
    }
    return head;
}

List Read()
{
    int num = 0;
    scanf("%d", &num);
    if(num < 0) {
        return NULL;
    }

    return CreateList(num);
}

void Print( List L )
{
    PtrToNode ptr;

    if(!L) return;

    ptr = L->Next;
    while(ptr) {
        printf("%d", ptr->Element);
        ptr = ptr->Next;
        if(ptr) 
            printf(" ");
    }
    printf("\n");
}

List Reverse( List L )
{
    PtrToNode head, tail, next;

    if(!L->Next) return L;

    head = L->Next;
    tail = head->Next;
    if(!tail) return L;

    next = tail->Next;
    head->Next = NULL;

    while(tail) {
        tail->Next = head;
        head = tail;
        tail = next;
        if(!next) { 
            L->Next = head;
            return L;
        }
        next = next->Next;
    }
    return NULL;
}
/*
List Reverse( List L )
{
    PtrToNode ptr = L->Next;
    PtrToNode tmp;

    L->Next = NULL;

    while(ptr) {
        tmp = ptr->Next;
        ptr->Next = L->Next;
        L->Next = ptr;
        ptr = tmp;
    }
    return L;
}
*/
