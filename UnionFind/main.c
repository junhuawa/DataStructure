#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int *parent;
int Num;

void Union(int root1, int root2);
int Find(int a);
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
int DealCmd(void)
{
    char cmd;
    int num1, num2;
    while(1) {
        scanf("%c", &cmd);
        if(cmd == 'I') {
            scanf("%d %d", &num1, &num2);
            Combine(num1, num2);
        }
        if(cmd == 'C') {
            scanf("%d %d", &num1, &num2);
            if(Find(num1) == Find(num2)) {
                printf("yes\n");
            }else {
                printf("no\n");
            }
        }
        if(cmd == 'S') {
            break;
        }
    }
    return 0;
}

int main(void)
{
    int num;
    scanf("%d", &num);
    if(num <= 1) return -1;
    Init(num);

    DealCmd();

    num = GetComponents();
    if(num == 1) {

        printf("The network is connected.\n");
    }else {
        printf("There are %d components.\n", num);
    }
    Free();
    return 0;
}
