#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

enum KindOfEntry
{
    Legitimate, 
    Empty, 
    Delete
};

struct HashEntry
{
    ElementType Element;
    enum KindOfEntry Info;
};

typedef struct HashEntry Cell;

struct HashTbl
{
    int TableSize;
    Cell *TheCells;
};

HashTable InitializeTable(int TableSize)
{
    return NULL;
}

void DestroyTable(HashTable H)
{

}

Position Find(ElementType key, HashTable H)
{
    return 0;
}

void Insert(ElementType key, HashTable H)
{
    
}

typedef void (* test) (void);

test tests[] = 
{
    0
};

int main(void)
{
    int i=0;
    while(tests[i]) {
        tests[i++]();
    }
    printf("%d tests passed\n", i);
    return 0;
}
