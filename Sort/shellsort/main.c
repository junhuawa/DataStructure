#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int ElementType;

void PrintData(ElementType *A, int N, int Increment)
{
    int i;
    printf("Increment: %d\n", Increment);
    for(i=0; i<N; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");
    return;
}

void Shellsort(ElementType A[], int N)
{
    int i, j, Increment;
    ElementType Tmp;

    for(Increment = N/2; Increment > 0; Increment /= 2)
    {
        for(i = Increment; i<N; i++)
        {
            Tmp = A[i];
            for(j = i; j>=Increment; j-=Increment)
                if(Tmp < A[j-Increment])
                    A[j] = A[j-Increment];
                else
                        break;
            A[j] = Tmp;
        }
    PrintData(A, N, Increment);
    }
}

int main(void)
{
    int i;
    int data[] = {49, 38, 65, 97, 76, 13, 27, 49, 55, 4 };
    int data1[] = {81, 94, 11, 96, 12, 35, 17, 95, 28, 58, 41, 75, 15 };
    Shellsort(data, 10);
    for(i=0; i<10; i++)
        printf("%d ", data[i]);
        printf("\n");

    Shellsort(data1, 13);
    for(i=0; i<13; i++)
        printf("%d ", data1[i]);
        printf("\n");
    return 0;
}

