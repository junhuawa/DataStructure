#ifndef __SATCK_H__
#define __STACK_H__

#define ERROR 1e8
typedef int ElementType;
typedef enum { push, pop, end } Operation;
enum {STACK_DUMMY, STACK1, STACK2, STACKNONE};

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

#endif

