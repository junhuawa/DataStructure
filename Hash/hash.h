#ifndef __HASH_TABLE__
#define __HASH_TABLE__
#include <stdbool.h>
typedef unsigned int Index;
typedef Index Position;
typedef int ElementType;

struct HashTbl;
typedef struct HashTbl * HashTable;

HashTable InitializeTable(int TableSize);
void DestroyTable(HashTable H);
Position Find(ElementType key, HashTable H);
bool Insert(ElementType key, HashTable H);




#endif
