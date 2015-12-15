#include <string.h>
#include <stdio.h>
#include "hash.h"

int main(void)
{
    int size, num;
    int v;
    bool rc;
    HashTable ptr = NULL;

    scanf("%d %d", &size, &num);
    ptr = InitializeTable(size);
    while(num>0) {
        scanf("%d", &v);
        rc = Insert(v, ptr);
        if(!rc) {
            printf("-");
        }else {
            printf("%d", Find(v, ptr));
        }
        num --;
        if(num > 0) {
            printf(" ");
        }
    }
    printf("\n");
    DestroyTable(ptr);
    return 0;
}
