#ifndef __DEQUE_H__
#define __DEQUE_H__

#include <stdbool.h>
#define ElementType int
#define ERROR 1e5
typedef enum { push, pop, inject, eject, end } Operation;

struct DequeRecord;
typedef struct DequeRecord *Deque;

Deque CreateDeque();
int Push( ElementType X, Deque D );
ElementType Pop( Deque D );
int Inject( ElementType X, Deque D );
ElementType Eject( Deque D );

void DisposeDeque(Deque D);
#endif
