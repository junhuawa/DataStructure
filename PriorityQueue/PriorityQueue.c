#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "PriorityQueue.h"

#define MinData -1
struct HeapStruct {
    ElementType  *Elements;
    int Capacity;
    int Size;
};

PriorityQueue Initialize( int MaxElements )
{
	PriorityQueue  queue = NULL;
	queue = malloc(sizeof(struct HeapStruct));
	if(!queue){
		printf("Malloc failed\n");
		return NULL;
	}
	queue->Size = 0;
	queue->Capacity = MaxElements;
	queue->Elements = malloc((MaxElements+1) * sizeof(ElementType));
	if(!queue->Elements){
		printf("Malloc failed\n");
		return NULL;
	}
    queue->Elements[0] = MinData;
	return queue;
}


void Free(PriorityQueue H)
{
	if(!H) return;
	if(H->Elements){
		free(H->Elements);
	}
	free(H);
	return ;
}

bool IsFull(PriorityQueue H);
bool IsEmpty(PriorityQueue H);

void PercolateUp(ElementType p, PriorityQueue H );
void PercolateDown(ElementType p, PriorityQueue H );

void Insert( ElementType X, PriorityQueue H ) 
{
    int p;
    if(IsFull(H)) {
        printf("Queue is already full!\n");
        return;
    }
    p = ++H->Size;
    H->Elements[p] = X;
    PercolateUp( p, H );
}

void BuildHeap(ElementType data[], int len, PriorityQueue H)
{
    int i;

    if(len > H->Capacity) {
        printf("data is too large!\n");
        return;
    }

    memcpy(H->Elements+1, data, len*sizeof(ElementType));
    H->Size = len;
    for(i=len/2; i > 0; i--) {
        PercolateDown(i, H);
    }
}

ElementType DeleteMin( PriorityQueue H ) 
{ 
    ElementType MinElement; 
    if(IsEmpty(H)) {
        printf("Queue is empty!\n");
        return H->Elements[0];
    }
    MinElement = H->Elements[1];
    H->Elements[1] = H->Elements[H->Size--];
    PercolateDown( 1, H );
    return MinElement; 
}

void PercolateUp( int p, PriorityQueue H )
{
    int i;
    ElementType X = H->Elements[p];
    for(i=p; H->Elements[i/2] > X; i=i/2)
        H->Elements[i] = H->Elements[i/2];
    H->Elements[i] = X;
}

void PercolateDown( int p, PriorityQueue H )
{
    int i, child;
    int LastElement = H->Elements[p];

    for(i=p; (i*2 <= H->Size); i=child) {
        child = i*2;
        if((child != H->Size) && (H->Elements[child+1]<H->Elements[child]))
            child ++;
        if(LastElement > H->Elements[child]) {
            H->Elements[i] = H->Elements[child];
        }else {
            break;
        }
    }
    H->Elements[i] = LastElement;
}

bool IsFull(PriorityQueue H){
    return H->Size == H->Capacity;
}

bool IsEmpty(PriorityQueue H){
    return H->Size == 0;
}

#ifdef __UT__
typedef void (*test)(void);

void TestQueueCreateSuccessfully(void){

	PriorityQueue queue = Initialize(100);
	assert(queue->Size == 0);
	assert(queue->Capacity == 100);
	assert(queue->Elements != NULL);
	Free(queue);
}

void TestQueueInsertTwoItem(void){

	PriorityQueue queue = Initialize(100);
	Insert(10, queue);
	Insert(2, queue);
	assert(queue->Size == 2);
	assert(queue->Elements[1]  == 2);	
	assert(queue->Elements[2]  == 10);	
	Free(queue);
}

void TestQueueInsert15Item(void){

    int i;
    int input[] = {10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2};
    int output[] = {-1, 1,3,2,6,7,5,4,15,14,12,9,10,11,13,8};

	PriorityQueue queue = Initialize(100);
    for(i=0; i<15; i++) { 
        Insert(input[i], queue);
    }

    for(i=1; i<16; i++) {
        assert(queue->Elements[i] == output[i]);
    }
	assert(queue->Size == 15);
	Free(queue);
}

void TestQueueDeleteOneItem(void){

    int i;
    int input[] = {10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2};
    int output[] = {-1, 2,3,4,6,7,5,8,15,14,12,9,10,11,13};

	PriorityQueue queue = Initialize(100);
    for(i=0; i<15; i++) { 
        Insert(input[i], queue);
    }

    assert(1 == DeleteMin(queue));
    for(i=1; i<14; i++) {
        assert(queue->Elements[i] == output[i]);
    }
	assert(queue->Size == 14);
	Free(queue);
}

void TestQueueDelete3Item(void){

    int i;
    int input[] = {10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2};
    int output[] = {-1, 4,6,5,13,7,10,8,15,14,12,9,11};

	PriorityQueue queue = Initialize(100);
    for(i=0; i<15; i++) { 
        Insert(input[i], queue);
    }

    assert(1 == DeleteMin(queue));
    assert(2 == DeleteMin(queue));
    assert(3 == DeleteMin(queue));
    for(i=1; i<12; i++) {
        assert(queue->Elements[i] == output[i]);
    }
	assert(queue->Size == 12);
	Free(queue);
}

void TestQueueBuildHeap(void){

    int i;
    int input[] = {10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2};
    int output[] = {-1, 1,3,2,12,6,4,8,15,14,9,7,5,11,13,10};

	PriorityQueue queue = Initialize(100);
    BuildHeap(input, 15, queue);

   for(i=1; i<15; i++) {
       assert(queue->Elements[i] == output[i]);
   }
	assert(queue->Size == 15);
	Free(queue);
}

void TestQueueBuildHeapAndDelete3Item(void){

    int i;
    int input[] = {10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2};
    int output[] = {-1,4,6,5,12,7,10,8,15,14,9,13,11};

	PriorityQueue queue = Initialize(100);
    BuildHeap(input, 15, queue);

    assert(1 == DeleteMin(queue));
    assert(2 == DeleteMin(queue));
    assert(3 == DeleteMin(queue));
   for(i=1; i<12; i++) {
       assert(queue->Elements[i] == output[i]);
   }
	assert(queue->Size == 12);
	Free(queue);
}

static test tests[] = {
	TestQueueCreateSuccessfully,
	TestQueueInsertTwoItem,
    TestQueueInsert15Item,
    TestQueueDeleteOneItem,
    TestQueueDeleteOneItem,
    TestQueueDelete3Item,
    TestQueueBuildHeap,
    TestQueueBuildHeapAndDelete3Item,
    (test) 0,
};

int main(void){
	int at= 0;
	while(tests[at]){
		tests[at++]();
	}
	printf("%d tests passed\n", at);
	return 0;
}
#endif
