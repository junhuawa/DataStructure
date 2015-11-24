#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "Stack.h"

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

#ifdef __UT__
typedef void test(void);

void Stack_is_Empty()
{
    Stack S;
    S = CreateStack(5);
    assert(IsEmpty(S, STACK1) == true); 
    assert(IsEmpty(S, STACK2) == true); 
    DisposeStack(S);
}

void Stack_is_Full_by_Push_Stack1()
{
    Stack S;
    bool rc;

    S = CreateStack(5);
    Push(1, S, STACK1);
    Push(2, S, STACK1);
    Push(3, S, STACK1);
    Push(4, S, STACK1);
    rc = Push(5, S, STACK1);
    assert(rc == true);
    rc = Push(6, S, STACK1);
    assert(rc == false);
    
    assert(IsEmpty(S, STACK1) == false); 
    assert(IsEmpty(S, STACK2) == true); 
    assert(IsFull(S) == true); 
    DisposeStack(S);
}

void Stack_is_not_Full_by_Push_Stack1()
{
    Stack S;
    S = CreateStack(5);
    Push(1, S, STACK1);
    Push(2, S, STACK1);
    Push(3, S, STACK1);
    
    assert(IsEmpty(S, STACK1) == false); 
    assert(IsEmpty(S, STACK2) == true); 
    assert(IsFull(S) == false); 
    DisposeStack(S);
}

void Stack_is_Full_by_Push_Stack2()
{
    Stack S;
    bool rc;

    S = CreateStack(5);
    Push(1, S, STACK2);
    Push(2, S, STACK2);
    Push(3, S, STACK2);
    Push(4, S, STACK2);
    rc = Push(5, S, STACK2);
    assert(rc == true);
    rc = Push(6, S, STACK2);
    assert(rc == false);
    
    assert(IsEmpty(S, STACK1) == true); 
    assert(IsEmpty(S, STACK2) == false); 
    assert(IsFull(S) == true); 
    DisposeStack(S);
}

void Stack_is_Full_by_Push_Stack2_and_Stack1()
{
    Stack S;
    bool rc;

    S = CreateStack(5);
    Push(1, S, STACK1);
    Push(2, S, STACK1);
    Push(3, S, STACK2);
    Push(4, S, STACK2);
    rc = Push(5, S, STACK2);
    assert(rc == true);
    rc = Push(6, S, STACK2);
    assert(rc == false);
    
    assert(IsEmpty(S, STACK1) == false); 
    assert(IsEmpty(S, STACK2) == false); 
    assert(IsFull(S) == true); 
    DisposeStack(S);
}

void Stack_Push_is_true_when_Empty()
{
    Stack S;
    bool rc;
    S = CreateStack(5);
    rc = Push(1, S, STACK1);
    
    assert(rc == true); 
    DisposeStack(S);
}

void Stack_Pop_Correct_value()
{
    Stack S;
    ElementType value;

    S = CreateStack(5);

    Push(1, S, STACK1);
    Push(2, S, STACK1);
    Push(3, S, STACK2);
    Push(4, S, STACK2);
    Push(5, S, STACK2);

    value = Top_Pop(S, STACK1);
    assert(value == 2);
    value = Top_Pop(S, STACK1);
    assert(value == 1);
    value = Top_Pop(S, STACK1);
    assert(value == ERROR);

    value = Top_Pop(S, STACK2);
    assert(value == 5);
    value = Top_Pop(S, STACK2);
    assert(value == 4);
    value = Top_Pop(S, STACK2);
    assert(value == 3);
    value = Top_Pop(S, STACK2);
    assert(value == ERROR);
    
    assert(IsEmpty(S, STACK1) == true); 
    assert(IsEmpty(S, STACK2) == true); 
    assert(IsFull(S) == false); 
    DisposeStack(S);
}

void Stack_Print_Correct()
{
    Stack S;
    S = CreateStack(5);
    Push(1, S, STACK1);
    Push(2, S, STACK1);
    Push(3, S, STACK1);
    Push(4, S, STACK1);
    Push(5, S, STACK1);
    
    PrintStack(S, STACK1);
    PrintStack(S, STACK2);
    DisposeStack(S);
}

static test *tests[] =
{
    Stack_is_Empty,
    Stack_Push_is_true_when_Empty,
    Stack_is_Full_by_Push_Stack1,
    Stack_is_not_Full_by_Push_Stack1,
    Stack_is_Full_by_Push_Stack2,
    Stack_is_Full_by_Push_Stack2_and_Stack1,
    Stack_Pop_Correct_value,
    Stack_Print_Correct,
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
