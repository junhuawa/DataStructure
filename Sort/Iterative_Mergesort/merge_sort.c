#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ElementType int
#define MAXN 100

void merge_pass( ElementType list[], ElementType sorted[], int N, int length );

void output( ElementType list[], int N )
{
    int i;
    for (i=0; i<N; i++) printf("%d ", list[i]);
    printf("\n");
}

void  merge_sort( ElementType list[],  int N )
{
    ElementType extra[MAXN];  /* the extra space required */
    int  length = 1;  /* current length of sublist being merged */
    while( length < N ) { 
        merge_pass( list, extra, N, length ); /* merge list into extra */
        output( extra, N );
        length *= 2;
        merge_pass( extra, list, N, length ); /* merge extra back to list */
        output( list, N );
        length *= 2;
    }
} 

void merge( ElementType list[], ElementType sorted[], int Lpos, int LeftEnd, int Rpos, int RightEnd){
	int pos =0;
	while(Lpos < LeftEnd && Rpos < RightEnd){
		if(list[Lpos] < list[Rpos]){
			sorted[pos++] = list[Lpos++];
		}else{
			sorted[pos++] = list[Rpos++];
		}
	}
	if(Lpos == LeftEnd){
		while(Rpos <RightEnd){
			sorted[pos++] = list[Rpos++];
		}
	}
	if(Rpos == RightEnd){
		while(Lpos<LeftEnd){
			sorted[pos++] = list[Lpos++];
		}
	}
}

void merge_pass( ElementType list[], ElementType sorted[], int N, int length ){
	int round;
	int i;
	int remain = N%(2*length);
	round = N/(2*length);
	if(round > 0){
		for(i=0; i<round; i++){
			merge(&(list[i*2*length]), &(sorted[i*2*length]), 0, length, length, 2*length);
		}
	}
	if(remain != 0){
		if(remain > length){
			merge(&(list[round*2*length]), &(sorted[round*2*length]), 0, length, length, remain);	
		}else{
			merge(&(list[round*2*length]), &(sorted[round*2*length]), 0, remain, 0, 0);	

		}
	}


}

typedef void (*test)(void);

void Test_OnePass(void){
	int list [] = {8, 7, 9, 2, 3, 5, 1, 6, 4, 0};
	int sorted[10];
	int expected[10]={7, 8, 2, 9, 3, 5, 1, 6, 0, 4};
	int i;

	merge_pass(list, sorted, 10, 1);
	
	
	output(list, 10);
	output(sorted, 10);
	for(i=0; i<10; i++){
		assert(expected[i] == sorted[i]);	
	}
}

void Test_TwoPass(void){
	int list [] = {8, 7, 9, 2, 3, 5, 1, 6, 4, 0};
	int sorted[10];
	int expected[10]={2, 7, 8, 9, 1, 3, 5, 6, 0, 4};
	int i;

	merge_pass(list, sorted, 10, 1);
	merge_pass(sorted, list, 10, 2);
	
	
	output(list, 10);
	output(sorted, 10);
	for(i=0; i<10; i++){
		assert(expected[i] == list[i]);	
	}
}

void Test_ThreePass(void){
	int list [] = {8, 7, 9, 2, 3, 5, 1, 6, 4, 0};
	int sorted[10];
	int expected[10]={1, 2, 3, 5, 6, 7, 8, 9, 0, 4};
	int i;

	merge_pass(list, sorted, 10, 1);
	merge_pass(sorted, list, 10, 2);
	merge_pass(list, sorted, 10, 4);
	
	
	output(list, 10);
	output(sorted, 10);
	for(i=0; i<10; i++){
		assert(expected[i] == sorted[i]);	
	}
}

void Test_FourPass(void){
	int list [] = {8, 7, 9, 2, 3, 5, 1, 6, 4, 0};
	int sorted[10];
	int expected[10]={0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int i;

	merge_pass(list, sorted, 10, 1);
	merge_pass(sorted, list, 10, 2);
	merge_pass(list, sorted, 10, 4);
	merge_pass(sorted, list, 10, 8);
	
	
	output(list, 10);
	output(sorted, 10);
	for(i=0; i<10; i++){
		assert(expected[i] == list[i]);	
	}
}
/*

7 8 2 9 3 5 1 6 0 4 
2 7 8 9 1 3 5 6 0 4 
1 2 3 5 6 7 8 9 0 4 
0 1 2 3 4 5 6 7 8 9 
0 1 2 3 4 5 6 7 8 9
*/
test tests[] = {
	Test_OnePass,
	Test_TwoPass,
	Test_ThreePass,
	Test_FourPass,
	(test) 0,
};

int main(void){
	int i=0;
	while(tests[i]){
		printf("case: %d\n", i);
		tests[i++]();
	}
	printf("%d tests passed\n", i);
	return 0;
}
