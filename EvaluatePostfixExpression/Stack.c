#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "Stack.h"

int IsEmpty(Stack S)
{
    if(!S) return -1;
    if(!(S->Next)) return true;
    return false;
}

Stack CreateStack(void)
{
    Stack S = malloc(sizeof(struct Node));
    if(!S) {
        printf("malloc failed\n");
        return NULL;
    }
    S->Next = NULL;
    S->Element = NULL;
    return S;
}

void DisposeStack(Stack S)
{
    PtrToNode ptr;
    PtrToNode tmp;

    if(!S) return; 
    ptr = S->Next;
    while(ptr) {
        tmp = ptr->Next;
        free(ptr);
        ptr = tmp;
    }
    free(S);
    S = NULL;
    return;
}

void MakeEmpty(Stack S)
{
    PtrToNode ptr, tmp;

    if(!S) return; 
    ptr = S->Next;
    while(ptr) {
        tmp = ptr->Next;
        free(ptr);
        ptr = tmp;
    }
    S->Next = NULL;
    return;
}

void Push(Tree X, Stack S)
{
    PtrToNode tmp = malloc(sizeof(struct Node));
    if(!tmp) {
        printf("malloc failed\n");
        return;
    }

    tmp->Element = X;
    tmp->Next = S->Next;
    S->Next = tmp;
    return;
}

Tree Top(Stack S)
{
    if(S->Next) {
        return (S->Next)->Element;
    }
    return NULL;
}

void Pop(Stack S)
{
    PtrToNode ptr = S->Next;
    if(S->Next) {
        ptr = (S->Next)->Next;
        free(S->Next);
        S->Next = ptr;
    }
    return;
}

Tree TopAndPop(Stack S)
{
    Tree v = Top(S);
    Pop(S);

    return v;
}

