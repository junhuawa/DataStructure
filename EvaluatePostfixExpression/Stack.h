#ifndef __STACK_H__
#define __STACK_H__
#include "common.h"

//typedef Tree ElementType;

typedef struct Node *PtrToNode;

struct Node
{
    Tree Element;
    PtrToNode Next;
};

typedef PtrToNode Stack;


int IsEmpty(Stack S);
Stack CreateStack(void);
void DisposeStack(Stack S);
void MakeEmpty(Stack S);
void Push(Tree X, Stack S);
Tree Top(Stack S);
void Pop(Stack S);
Tree TopAndPop(Stack S);

#endif
