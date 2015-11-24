#include <stdio.h>
#include <stdlib.h>
#define ERROR 1e8
typedef int ElementType;
typedef enum { push, pop, end } Operation;

typedef struct StackRecord *Stack;
struct StackRecord  {
    int Capacity;       /* maximum size of the stack array */
    int Top1;           /* top pointer for Stack 1 */
    int Top2;           /* top pointer for Stack 2 */
    ElementType *Array; /* space for the two stacks */
};

Stack CreateStack( int MaxElements );
int IsEmpty( Stack S, int Stacknum );
int IsFull( Stack S );
int Push( ElementType X, Stack S, int Stacknum );
ElementType Top_Pop( Stack S, int Stacknum );

Operation GetOp();  /* details omitted */
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
    if(strncmp("End", buf, 4) == 0) {
        return end;
    }
    return end;
}

void PrintStack( Stack S, int Stacknum ); /* details omitted */

void PrintStack( Stack S, int Stacknum )
{
    ElementType value;
    printf("Pop from Stack %d: ", Stacknum);
    value = Top_Pop(S, Stacknum);
    while(value != ERROR) {
        printf("%d", value);
        if(!IsEmpty(S, Stacknum)) {
            printf(" ");
        }
        value = Top_Pop(S, Stacknum);
    }
    printf("\n");
    return;
}
int main()
{
    int N, Sn, X;
    Stack S;
    int done = 0;

    scanf("%d", &N);
    S = CreateStack(N);
    while ( !done ) {
        switch( GetOp() ) {
        case push: 
            scanf("%d %d", &Sn, &X);
            if (!Push(X, S, Sn)) printf("Stack %d is Full!\n", Sn);
            break;
        case pop:
            scanf("%d", &Sn);
            X = Top_Pop(S, Sn);
            if ( X==ERROR ) printf("Stack %d is Empty!\n", Sn);
            break;
        case end:
            PrintStack(S, 1);
            PrintStack(S, 2);
            done = 1;
            break;
        }
    }
    return 0;
}

enum {STACK_DUMMY, STACK1, STACK2, STACKNONE};
#include <stdbool.h>
#include <string.h>
#include <assert.h>

Stack CreateStack( int MaxElements )
{
    Stack stack = NULL;
    if(MaxElements <= 0) return NULL;
    stack = malloc(sizeof(struct StackRecord));
    if(!stack) {
        return NULL;
    }
    stack->Array = malloc(sizeof(ElementType) * MaxElements);
    if(!stack->Array) {
        free(stack);
        return NULL;
    }
    stack->Top1 = -1;
    stack->Top2 = -1;
    stack->Capacity = MaxElements;

    return stack;    
}

int IsEmpty( Stack S, int Stacknum )
{
    if(!S) {
        exit(1);
    }
    if(Stacknum == STACK1) 
        return (S->Top1 == -1);
    if(Stacknum == STACK2) 
        return (S->Top2 == -1);
    return false;
}

int IsFull( Stack S )
{
    int total1 = S->Top1 + 1;
    int total2 = S->Top2 + 1;
    return(total1+total2 == S->Capacity);
}

int Push( ElementType X, Stack S, int Stacknum )
{
    if(IsFull(S) == true) {
        return false;
    }

    if(Stacknum == STACK1) {
        S->Array[++S->Top1] = X;
        return true;
    }

    if(Stacknum == STACK2) {
        S->Array[S->Capacity-1 -(++S->Top2)] = X;
        return true;
    }
    return 0;
}

ElementType Top_Pop_Stack1(Stack S)
{
    if(!IsEmpty(S, STACK1)) {
        return S->Array[S->Top1 --];
    }
    return ERROR;
}

ElementType Top_Pop_Stack2(Stack S)
{
    int tmp;
    if(!IsEmpty(S, STACK2)) {
        tmp = S->Array[S->Capacity-1-S->Top2];
        S->Top2 --;
        return tmp;
    }
    return ERROR;
}

ElementType Top_Pop( Stack S, int Stacknum )
{
    if(Stacknum == STACK1) {
        return Top_Pop_Stack1(S);
    }
    if(Stacknum == STACK2) {
        return Top_Pop_Stack2(S);
    }
    return ERROR;
}

void DisposeStack(Stack S)
{
    if(!S) {
        if(S->Array) {
            free(S->Array);
        }
        free(S);
    }
}

