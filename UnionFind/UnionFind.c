#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "UnionFind.h"

//#define NUM (10000+1)
int *parent;
int Num;

bool Combine(int a, int b)
{
    int root1, root2;
    root1 = Find(a);
    root2 = Find(b);
    if(root1 != root2) {
        Union(root1, root2);
    }
    return true;
}

int Find(int a)
{
    if(parent[a] <= 0) {
        return a;
    }
    return (parent[a] = Find(parent[a]));
}

/*
int Find(int a)
{
    while(parent[a] > 0) {
       a = parent[a];
    }

    return a;
}
*/

void Union(int root1, int root2)
{
    int num1, num2;
    num1 = - parent[root1];
    num2 = - parent[root2];

    if(num1 > num2) {
        num1 += num2;
        parent[root2] = root1;
        parent[root1] = -num1;
        return;
    }

    num2 += num1;
    parent[root1] = root2;
    parent[root2] = -num2;
}

int GetComponents(void)
{
    int i;
    int count = 0;
    for(i=1; i<Num+1; i++) {
        if(parent[i] <= 0) {
            count ++;
        }
    }

    return count;
}

void Init(int num)
{
   int i;
   parent = malloc((num+1)* sizeof(int));
   if(!parent) {
        printf("malloc error\n");
        return;
   }

   for(i=0; i<num+1; i++) {
        parent[i] = -1;
   }
   Num = num;
   return; 
}

void Free(void)
{
    if(parent) {
        free(parent);
    }
    parent = NULL;
}


void dump(int data[])
{
    int i;
    for(i=0; i<100; i++) {
        printf("%d ", data[i]);
        if((i+1)%16==0) {
            printf("\n");
        }
    }
    printf("\n");
}

#ifdef __UT__
typedef void (*test)();

void TestInitTheUnionFindArray(void)
{
    int i;
    int num = 100;
    Init(num);
    for(i=0; i<num+1; i++) {
        assert(parent[i] == -1);
    }
    Free();
}

void TestInitFind(void)
{
    int i;
    int num = 100;
    Init(num);
    for(i=1; i<num+1; i++) {
        assert(Find(i) == i);
    }
    Free();
}

void TestCombine(void)
{
    Init(100);
    Combine(3, 4);
    //dump(parent);
    assert(Find(3) == 4);
    assert(Find(4) == 4);
    assert(parent[4] == -2);
    
    Combine(5, 6);
    assert(Find(5) == 6);
    assert(Find(6) == 6);
    Combine(7, 8);
    Combine(3, 7);
    
    assert(Find(3) == 8);
    assert(Find(4) == 8);
    assert(Find(7) == 8);
    assert(Find(8) == 8);
    assert(parent[8] == -4);
    
    Combine(6, 7);
    assert(Find(5) == 8);
    assert(Find(6) == 8);
    assert(Find(3) == Find(7));
    assert(parent[8] == -6);

    //dump(parent);
    
    Combine(6, 12);
    //dump(parent);
    assert(Find(12) == 8);
    assert(parent[8] == -7);
    
    Free();
}

void TestBinomialCombine(void)
{
    Init(100);
    Combine(63, 64);
    Combine(61, 62);
    Combine(59, 60);
    Combine(57, 58);

    Combine(62, 63);
    Combine(57, 59);
    Combine(57, 63);

    Combine(55, 56);
    Combine(53, 54);
    Combine(51, 52);
    Combine(49, 50);

    Combine(53, 56);
    Combine(49, 51);
    Combine(49, 56);
    assert(parent[49] == 52);
    assert(parent[50] == 52);
    assert(parent[52] == 56);
    assert(parent[56] == -8);

    Combine(49, 63);
    assert(parent[49] == 56);

    assert(Find(49) == 64);

    assert(parent[49] == 64);
    //dump(parent);
    assert(parent[50] == 52);
    assert(parent[52] == 56);
    assert(parent[56] == 64);
    Free();
}

void TestGetComponent(void)
{
    Init(10);
    assert(GetComponents() == 10);
    Free();
}

void TestGetComponentOnlyTwo(void)
{
    Init(5);
    Combine(3, 2);
    Combine(1, 5);
    Combine(3, 5);
    Combine(1, 5);

//    printf("components: %d\n", GetComponents());
    assert(GetComponents() == 2);
    Free();
}

void TestGetComponentOnlyOne(void)
{
    Init(5);
    Combine(3, 2);
    Combine(1, 5);
    Combine(3, 5);
    Combine(1, 5);
    Combine(4, 5);

//    printf("components: %d\n", GetComponents());
    assert(GetComponents() == 1);
    Free();
}

test tests[] = 
{
    TestInitTheUnionFindArray,
    TestInitFind,
    TestCombine,
    TestBinomialCombine,
    TestGetComponent,
    TestGetComponentOnlyTwo,
    TestGetComponentOnlyOne,
    (test)0,
};


int main(void)
{
    int i=0;
    while(tests[i])
    {
        tests[i++]();
    }
    printf("%d cases passed\n", i);
    return 0;
}

#endif
