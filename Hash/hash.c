#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
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

bool IsPrime(int num)
{
    int i;
    for(i=2; i<=sqrt(num); i++) {
        if(num %i == 0)
            return false;
    }
   return true; 
}

int NextPrime(int TableSize)
{
    int num = TableSize;
    while(1) {
        if(IsPrime(num)) {
            return num;
        }
        num ++;
    }
}

HashTable InitializeTable(int TableSize)
{
    HashTable ptr = NULL;
    int num;
    int i;

    if(TableSize <= 2) return NULL;
    num = NextPrime(TableSize);
    ptr = malloc(sizeof(struct HashTbl));
    if(!ptr) {
        printf("Malloc failed\n");
        return NULL;
    }
    ptr->TableSize = num;
    ptr->TheCells = malloc(sizeof(struct HashEntry) * ptr->TableSize);
    if(!ptr->TheCells) {
        printf("Malloc The Cells failed\n");
        free(ptr);
        return NULL;
    }
    for(i=0; i<ptr->TableSize; i++) {
        ptr->TheCells[i].Info = Empty;
    }
    return ptr;
}

void DestroyTable(HashTable H)
{
    if(!H) {
        return;
    }
    if(H->TheCells) {
        free(H->TheCells);
    }
    free(H);
    return;
}

Position Find(ElementType key, HashTable H)
{
    int pos;
    int originalPos;
    int ProbeCount = 0;

    if(!H) return -1;

    originalPos = key%(H->TableSize);
    pos = originalPos;
    while((H->TheCells[pos].Info != Empty)
            &&(H->TheCells[pos].Element != key)){
        ProbeCount ++;
        pos = (originalPos+ProbeCount*ProbeCount)%(H->TableSize);
        if(ProbeCount * ProbeCount >= H->TableSize) {
            return -1;
        }
    }

    return pos;
}

bool Insert(ElementType key, HashTable H)
{
    int pos;

    if(!H) return false;
    
    pos = Find(key, H);
    //printf("key: %d, pos: %d\n", key, pos);
    if(pos < 0) {
        return false;
    }

    H->TheCells[pos].Element = key;
    H->TheCells[pos].Info = Legitimate;
    return true;
}

#ifdef __UT__
typedef void (* test) (void);

void TestInitializeTable(void)
{
    int i;
    HashTable TablePtr = InitializeTable(100);

    assert(TablePtr->TableSize == 101);
    for(i=0; i<101; i++) {
        assert(TablePtr->TheCells[i].Info == Empty);
    }
    DestroyTable(TablePtr);

    TablePtr = InitializeTable(50);

    assert(TablePtr->TableSize == 53);
    for(i=0; i<53; i++) {
        assert(TablePtr->TheCells[i].Info == Empty);
    }
    DestroyTable(TablePtr);
}

void TestGetNextPrime(void)
{
    assert(NextPrime(4) == 5);
    assert(NextPrime(5) == 5);
    assert(NextPrime(6) == 7);
    assert(NextPrime(9) == 11);
    assert(NextPrime(20) == 23);
    assert(NextPrime(40) == 41);
    assert(NextPrime(42) == 43);
    assert(NextPrime(50) == 53);
}

void TestInsert(void)
{
    int i;
    HashTable TablePtr = InitializeTable(4);

    assert(TablePtr->TableSize == 5);
    for(i=0; i<5; i++) {
        assert(TablePtr->TheCells[i].Info == Empty);
    }

    Insert(10, TablePtr);
    Insert(6, TablePtr);
    Insert(4, TablePtr);
    assert(Insert(15, TablePtr)==false);

    assert(Find(10, TablePtr) == 0);
    assert(Find(6, TablePtr) == 1);
    assert(Find(4, TablePtr) == 4);
    assert(Find(15, TablePtr) == -1);

    DestroyTable(TablePtr);
}


void TestInsertWithProbe(void)
{
    int i;
    HashTable TablePtr = InitializeTable(10);

    assert(TablePtr->TableSize == 11);
    for(i=0; i<11; i++) {
        assert(TablePtr->TheCells[i].Info == Empty);
    }

    Insert(11, TablePtr);
    Insert(89, TablePtr);
    Insert(18, TablePtr);
    Insert(49, TablePtr);
    Insert(58, TablePtr);
    Insert(69, TablePtr);
    Insert(15, TablePtr);
    assert(Insert(25, TablePtr) == false);
    assert(Insert(34, TablePtr) == true);

    assert(Find(11, TablePtr) == 0);
    assert(Find(89, TablePtr) == 1);
    assert(Find(58, TablePtr) == 3);
    assert(Find(69, TablePtr) == 4);
    assert(Find(49, TablePtr) == 5);
    assert(Find(18, TablePtr) == 7);
    //assert(Find(25, TablePtr) == -1);
    assert(Find(34, TablePtr) == 2);
    assert(Find(15, TablePtr) == 8);

    DestroyTable(TablePtr);
}

test tests[] = 
{
    TestGetNextPrime,
    TestInitializeTable,
    TestInsert,
    TestInsertWithProbe,
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

#endif
