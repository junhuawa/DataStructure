#include <stdio.h>
#include <stdlib.h>
#include "Tree.h"
#include "Queue.h"

#define MaxTree 10 /* maximum number of nodes in a tree */

void PrintNode( Tree NodePtr )
{
   printf(" %d", NodePtr->Element);
}

void Level_order ( Tree T, void (*visit)(Tree ThisNode) ){
    Queue Q = CreateQueue();
    Tree tree;

    Enqueue(T, Q);
    while(!IsEmpty(Q)) { 
        tree = FrontAndDequeue(Q);
        if(tree) {
            Enqueue(tree->Left, Q);
            Enqueue(tree->Right, Q);
            visit(tree);
        }
    }
    printf("\n");
    DisposeQueue(Q);
    return;
}

int main()
{
    Tree T = BuildTree();
    printf("Level-order:");
    Level_order(T, PrintNode);
    return 0;
}
