#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Deque.h"

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
            if ( X==ERROR ) printf("Deque is Empty!\n");
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
    DisposeDeque(D);
    return 0;
}
