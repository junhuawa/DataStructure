#include <stdio.h>
#include <stdlib.h>

typedef char ElementType;

typedef struct TreeNode *Tree;
struct TreeNode {
    ElementType Element;
    Tree  Left;
    Tree  Right;
};

Tree BuildTree()
{
    return NULL;
}

int Isomorphic( Tree T1, Tree T2 );

int main()
{
    Tree T1, T2;
    T1 = BuildTree();
    T2 = BuildTree();
    printf("%d\n", Isomorphic(T1, T2));
    return 0;
}

#include <stdbool.h>
#include <string.h>

typedef struct Node *PtrToNode;
struct Node {
    Tree Element;
    PtrToNode Next, Last;
};
typedef struct QueueRecord *Queue;
struct QueueRecord {
    PtrToNode Front, Rear;
};
void Enqueue(Tree X, Queue Q);
Tree Front(Queue Q);
void Dequeue(Queue Q);
Tree FrontAndDequeue(Queue Q);
void DisposeQueue(Queue D);

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
    return q;
}

bool IsEmpty(Queue Q)
{
    if(!Q) exit(-1);
    return (Q->Front == Q->Rear);
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
        Q->Rear = Q->Front;
    }else {
        (ptr->Next)->Last = NULL;
    }
    Q->Front->Next = ptr->Next;

    value = ptr->Element;
    free(ptr);
    return value;
}

void Dequeue(Queue Q)
{
    PtrToNode ptr = NULL;

    if(!Q) exit(-1);
    if(IsEmpty(Q))return; 

    ptr = Q->Front->Next;
    if(IsOnlyOneElementLeft(ptr, Q)) {
        Q->Rear = Q->Front;
    }else {
        (ptr->Next)->Last = NULL;
    }
    Q->Front->Next = ptr->Next;

    free(ptr);
    return;
}

void RebuildTreeInOrder(Tree T);

int Isomorphic( Tree T1, Tree T2 )
{
    Queue Q1 = CreateQueue();
    Queue Q2 = CreateQueue();

    Tree tree1 = T1;
    Tree tree2 = T2;

    bool doExit = false;
    bool result = false;

    RebuildTreeInOrder(tree1);
    RebuildTreeInOrder(tree2);
//   PrintTreeInfixOrder(tree1);
//   printf("\n");
//   PrintTreeInfixOrder(tree2);
//   printf("\n");

    Enqueue(tree1, Q1);
    Enqueue(tree2, Q2);
    while((!IsEmpty(Q1)) && (!IsEmpty(Q2))) { 
        tree2 = FrontAndDequeue(Q2);
        tree1 = FrontAndDequeue(Q1);

        if(tree1 && (!tree2)) {
            result = false; 
            doExit = true;
            break;
        }
        if((!tree1) && tree2) {
            result = false; 
            doExit = true;
            break;
        }

        if(tree1 && tree2) {
            if(tree1->Element != tree2->Element) {
                result = false;
                doExit = true;
                break;
            }

            Enqueue(tree1->Left, Q1);
            Enqueue(tree1->Right, Q1);

            Enqueue(tree2->Left, Q2);
            Enqueue(tree2->Right, Q2);
        }
    }

    if(doExit) {
        DisposeQueue(Q1);
        DisposeQueue(Q2);
        return false;
    }

    if(IsEmpty(Q1) && IsEmpty(Q2)) {
        result = true;
    }

    DisposeQueue(Q1);
    DisposeQueue(Q2);
    return result;
}

void swap(Tree T)
{
    Tree tmp;

    tmp = T->Left;
    T->Left = T->Right;
    T->Right = tmp;
    return;
}

void RebuildTreeInOrder(Tree T)
{
    if(!T) return;
    if((!T->Left) && (!T->Right)) return;
    if((T->Left) && (T->Left->Element > T->Element)) {
        swap(T);
    }

    if((T->Right) && (T->Right->Element < T->Element)) {
        swap(T);
    }
    RebuildTreeInOrder(T->Left);
    RebuildTreeInOrder(T->Right);
}
