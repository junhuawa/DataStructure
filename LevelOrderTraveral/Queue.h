#ifndef __DEQUE_H__
#define __DEQUE_H__

#include <stdbool.h>
#include "common.h"
#define ERROR 1e5

typedef struct Node *PtrToNode;
struct Node {
    Tree Element;
    PtrToNode Next, Last;
};
typedef struct QueueRecord *Queue;
struct QueueRecord {
    PtrToNode Front, Rear;
};

bool IsEmpty(Queue Q);
Queue CreateQueue();

void Enqueue(Tree X, Queue Q);
Tree Front(Queue Q);
void Dequeue(Queue Q);
Tree FrontAndDequeue(Queue Q);
void DisposeQueue(Queue D);
#endif
