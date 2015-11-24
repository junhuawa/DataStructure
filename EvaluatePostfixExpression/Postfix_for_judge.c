#include <stdio.h>
#include <stdlib.h>

typedef double ElementType;
#define Infinity 1e8
#define Max_Expr 30    //max size of expression 

ElementType EvalPostfix( char *expr );

int main()
{
    ElementType v;
    char expr[Max_Expr];
    gets(expr);
    printf("%s\n", expr);
    v = EvalPostfix( expr );
    if ( v  != Infinity )
        printf("%f\n", v);
    else
        printf("Error\n");
    return 0;
}

#include <string.h>
#include <stdbool.h>


typedef struct Node *PtrToNode;

struct Node
{
    ElementType Element;
    PtrToNode Next;
};

typedef PtrToNode Stack;


int IsEmpty(Stack S);
Stack CreateStack(void);
void DisposeStack(Stack S);
void MakeEmpty(Stack S);
void Push(ElementType X, Stack S);
ElementType Top(Stack S);
void Pop(Stack S);
ElementType TopAndPop(Stack S);
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
    S->Element = -1;
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

void Push(ElementType X, Stack S)
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

ElementType Top(Stack S)
{
    if(S->Next) {
        return (S->Next)->Element;
    }
    return 0.0;
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

ElementType TopAndPop(Stack S)
{
    ElementType v = Top(S);
    Pop(S);

    return v;
}

Stack S = NULL;

enum {
    NORMAL,
    ERROR,
    NONE
};
int status = NORMAL;

bool IsOperator(char c)
{
    if( (c == '+') ||(c == '-')|| (c == '*') || (c == '/'))
        return true;
    return false;
}
ElementType DoOperation(char c)
{
    ElementType v1, v2;
    if(IsEmpty(S) == true) {
        status = ERROR;
        return Infinity;
    }
    v1 = TopAndPop(S);
    if(IsEmpty(S) == true) {
        status = ERROR;
        return Infinity;
    }
    v2 = TopAndPop(S);

    if(c == '+') {
        v1 = v2+v1;
        Push(v1, S);
        return v1;
    }

    if(c == '-') {
        v1 = v2-v1;
        Push(v1, S);
        return v1;
    }

    if(c == '*') {
        v1 = v2*v1;
        Push(v1, S);
        return v1;
    }

    if(c == '/') {
        if(v1 == 0.0) {
           status = ERROR; 
           return Infinity;
        }
        v1 = v2/v1;
        Push(v1, S);
        return v1;
    }
    return Infinity;
}

void OutputResult(S)
{
    return;
}

ElementType EvalPostfix( char *expr )
{
    ElementType v, value;
    int len = strlen(expr);
    int i=0;
    S = CreateStack();

    sscanf(expr, "%lf", &v);
    Push(v, S);
    while((expr[i] != ' ')  && (expr[i] != '\n')) i++;
    while((i < len)&&(status == NORMAL)) {
        while(expr[i] != ' ') i++;
        i++;
        if((IsOperator((char)(expr[i]))==true) && ((expr[i+1] == ' ')||(i+1 == len))) {
            value = DoOperation(expr[i]);
            i++;
            continue;
        }

        sscanf(expr+i, "%lf", &v);
        Push(v, S);
        i++;
    }
    if(status == NORMAL) {
        value = TopAndPop(S);
        if(IsEmpty(S) == false) {
            value = Infinity;
        }
    }
    DisposeStack(S);
    return value;
}
