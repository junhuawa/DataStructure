#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef enum { true, false, maybe } Keys;
typedef Keys ElementType;

void Read( ElementType A[], int N ); /* details omitted */

void MySort( ElementType A[], int N );

void PrintA( ElementType A[], int N )
{
    int i, k;

    k = i = 0;
    for ( ; i<N && A[i]==false; i++ );
    if ( i > k )
        printf("false in A[%d]-A[%d]\n", k, i-1);
    k = i;
    for ( ; i<N && A[i]==maybe; i++ );
    if ( i > k )
        printf("maybe in A[%d]-A[%d]\n", k, i-1);
    k = i;
    for ( ; i<N && A[i]==true; i++ );
    if ( i > k )
        printf("true in A[%d]-A[%d]\n", k, i-1);
    if ( i < N )
        printf("Wrong Answer\n");
}


void MySort( ElementType A[], int N )
{
    ElementType Bucket[maybe+1];
    int i;
    int j=0;

    memset(Bucket, 0, sizeof(Bucket));

    for(i=0; i<N; i++) {
        if(A[i] == false) {
        Bucket[0] ++;
        }else if(A[i] == maybe) {
            Bucket[1] ++;
        }else {
            Bucket[2] ++;
        }
    }

    for(i=0; i<maybe+1; i++) {
        while(Bucket[i] != 0) {
            A[j++] = i==0? false: (i==1?maybe: true);
            Bucket[i]--;
        }
    }
}

typedef void (*test)(void);

void TestSampleInput(void)
{
    int i;
    ElementType data[] = {2, 2, 0, 1, 0, 0};
    ElementType expected[] = {1, 2, 2, 0, 0, 0};
    MySort(data, 6);
    for(i=0; i<6; i++) {
        assert(data[i] == expected[i]);
    }

}

test tests[] = 
{
    TestSampleInput,
    (test)0,
};


int main()
{
    int i=0;

    while(tests[i]) {
        tests[i++]();
    }
    printf("%d tests passed\n", i);
    return 0;
}
/*
int main()
{
    int N;
    ElementType *A;

    scanf("%d", &N);
    A = (ElementType *)malloc(N * sizeof(ElementType));
    Read( A, N );
    MySort( A, N );
    PrintA( A, N );
    return 0;
}
*/
