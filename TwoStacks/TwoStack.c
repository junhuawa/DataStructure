#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Stack.h"



Operation GetOp();  
void PrintStack( Stack S, int Stacknum ); 

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

