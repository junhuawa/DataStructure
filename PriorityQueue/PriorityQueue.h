#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

typedef int ElementType;
typedef struct HeapStruct *PriorityQueue;

PriorityQueue Initialize( int MaxElements ); 
void Insert( ElementType X, PriorityQueue H );
ElementType DeleteMin( PriorityQueue H );
void BuildHeap(ElementType data[], int len, PriorityQueue H);

#endif
