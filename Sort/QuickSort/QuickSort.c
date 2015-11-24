#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CUTOFF 10
void InsertSort(int A[], int Left, int Right){
	int i;
	int j;
    int tmp;

	for(i=Left+1; i<=Right; i++) {
       tmp = A[i];
       for(j=i; j>Left; j--) {
            if(A[j-1] > tmp) {
                A[j] = A[j-1];
            }else {
                break;
            }
       }
       A[j] = tmp;
	}
}

void swap(int *p1, int *p2);
int Median3(int A[], int Left, int Right)
{
    int pos = (Left+Right)/2;
    if(A[Left] > A[Right]) {
        swap(&A[Left], &A[Right]);
    }
    if(A[pos] > A[Right]) {
       swap(&A[pos], &A[Right]); 
    }
    if(A[Left] > A[pos]) {
       swap(&A[pos], &A[Left]); 
    }
    swap(&A[pos], &A[Right-1]);
    return A[Right-1];
}

void swap(int *p1, int *p2)
{
    int tmp;

    tmp = *p1; 
    *p1 = *p2;
    *p2 = tmp;
    return;
}

void QuickSort(int A[], int Left, int Right){
	int i, j;
	int pivot;
	if(Left + CUTOFF < Right){
		pivot = Median3(A, Left, Right);
        printf("pivot: %d\n", pivot);
		i = Left;
		j = Right-1;
		for(;;){
			while(A[++i] < pivot){}
			while(A[--j] > pivot){}
            if(i<j) {
                swap(&A[i], &A[j]);
            }else {
                break;
            }
		}
		swap(&A[i], &A[Right-1]);
		QuickSort(A, Left, i-1);
		QuickSort(A, i+1, Right);
	}else{
		InsertSort(A, Left, Right);
		
	}
	
}

void Sort(int A[], int N){
	QuickSort(A, 0, N-1);
	
}

void Print(int data[], int N)
{
    int i;
    for(i=0; i<N; i++) {
        printf("%2d, ", data[i]);
    }
    printf("\n");
}

typedef void (*test)(void);
void TestInsertionSortDifferentNumber(void)
{
    int data[20] = {30, 20,  1,  2, 45, 43, 44, 69, 80, 10, 
                    22, 32, 65, 41, 47, 86, 49, 29, 40,  0};

    Print(data, 20);

    InsertSort(data, 0,20-1); 

    Print(data, 20);
}

void TestInsertionSortSomeSameNum(void)
{
    int data[20] = {30, 20,  1,  2, 45, 43, 44, 20, 40, 10, 
                    22, 20, 65, 41, 47, 86, 49, 20, 40,  0};

    Print(data, 20);

    InsertSort(data, 0,20-1); 

    Print(data, 20);
}

void TestQuickSortDifferentNumber(void)
{
    int data[100] = {30, 20,  1,  2, 45, 43, 44, 69, 80, 10, 
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     61, 64, 65, 67, 69, 95,  4,  2, 22,  41,
                     73, 75, 77, 42, 47, 53, 51, 59, 60,  66,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
    };

    Print(data, 100);

    Sort(data, 100); 
    //InsertSort(data, 0, 100-1); 

    Print(data, 100);
}

static int cmpint(const void *p1, const void *p2)
{
    return *(int*)p1 > *(int*)p2;
}

void TestQSortDifferentNumber(void)
{
    int data[100] = {30, 20,  1,  2, 45, 43, 44, 69, 80, 10, 
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     61, 64, 65, 67, 69, 95,  4,  2, 22,  41,
                     73, 75, 77, 42, 47, 53, 51, 59, 60,  66,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
                     22, 32, 65, 41, 47, 86, 49, 29, 40,   0,
    };

//    Print(data, 100);

    printf("By qsort from stdlib\n");
    qsort(data, 100, sizeof(int), cmpint); 
    //InsertSort(data, 0, 100-1); 

    Print(data, 100);
}
test tests[] = 
{
    TestInsertionSortDifferentNumber,
    TestInsertionSortSomeSameNum,
    TestQuickSortDifferentNumber,
    TestQSortDifferentNumber,
    (test)0,
};

int main(void)
{
    int i=0;
    i=0;
    while(tests[i]) {
        tests[i++]();
    }
    printf("%2d cases passed\n", i);

    return 0;
}
