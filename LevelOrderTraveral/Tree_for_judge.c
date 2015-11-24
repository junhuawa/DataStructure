
#include <stdio.h>
#include <stdlib.h>

#define MaxTree 10 /* maximum number of nodes in a tree */
typedef int ElementType;

typedef struct TreeNode *Tree;
struct TreeNode {
    ElementType Element;
    Tree  Left;
    Tree  Right;
};

Tree BuildTree()
{
    
}
/* details omitted */
void PrintNode( Tree NodePtr )
{
   printf(" %d", NodePtr->Element);
}

void Level_order ( Tree T, void (*visit)(Tree ThisNode) );

int main()
{
    Tree T = BuildTree();
    printf("Level-order:");
    Level_order(T, PrintNode);
    return 0;
}

#include <stdbool.h>
typedef struct Node *PtrToNode;
struct Node {
    Tree Element;
    PtrToNode Next, Last;
};
typedef struct QueueRecord *Queue;
struct QueueRecord {
    PtrToNode Front, Rear;
};

void Dequeue(Queue Q);
PtrToNode header = NULL;
Queue CreateQueue()
{
    Queue q = NULL;

    q = malloc(sizeof(struct QueueRecord));
    if(!q) {
        return NULL;
    }

    q->Front = malloc(sizeof(struct Node));
    if(!q->Front) {
        free(q);
        return NULL;
    }
    q->Front->Next = NULL;
    q->Front->Last = NULL;

    q->Rear = q->Front;
    header = q->Front;
    return q;
}

bool IsEmpty(Queue Q)
{
    if(!Q) exit(-1);
    return (Q->Front == Q->Rear)&&(Q->Front == header);
}

void DisposeQueue(Queue Q)
{
    if(!Q) return;

    while(!IsEmpty(Q)) {
        Dequeue(Q);
    }
    free(Q->Front);
    free(Q);
}


void Enqueue( Tree X, Queue Q)
{
    PtrToNode ptr = NULL;

    if(!Q) exit(-1);

    ptr = malloc(sizeof(struct Node));
    if(!ptr) exit(-1); 
    ptr->Element = X;
    ptr->Next = NULL;

    if(IsEmpty(Q)) {
        Q->Front->Next = ptr;
        ptr->Last = NULL;
    }else {
        Q->Rear->Next = ptr;
        ptr->Last = Q->Rear;
    }

    Q->Rear = ptr;
}

bool IsOnlyOneElementLeft(PtrToNode ptr, Queue Q)
{
    return ((Q->Rear == ptr)&&(Q->Front->Next == ptr));
}

Tree FrontAndDequeue(Queue Q)
{
    PtrToNode ptr = NULL;
    Tree value;

    if(!Q) exit(-1);
    if(IsEmpty(Q)) return NULL;

    ptr = Q->Front->Next;
    if(IsOnlyOneElementLeft(ptr, Q)) {
        Q->Rear = header;
    }else {
        (ptr->Next)->Last = NULL;
    }
    header->Next = ptr->Next;

    value = ptr->Element;
    free(ptr);
    return value;
}

void Dequeue(Queue Q)
{
    PtrToNode ptr = NULL;

    if(!Q) exit(-1);
    if(!IsEmpty(Q)) exit(-1);

    ptr = Q->Front->Next;
    if(IsOnlyOneElementLeft(ptr, Q)) {
        Q->Rear = header;
    }else {
        (ptr->Next)->Last = NULL;
    }
    header->Next = ptr->Next;

    free(ptr);
    return;
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
}

