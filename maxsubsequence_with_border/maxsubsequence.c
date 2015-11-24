#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <errno.h>

int get_max_sub_sequence_sum_and_border(int num, int *first, int *last)
{
    int i;
    int value;
    int tmpsum = 0;
    int maxsum = 0;

    int seqstart = 0;
    int seqend = 0;
    int ex_first = 0;
    bool first_zero_flag = false;
    bool all_negative_flag = true;

    for(i=0; i<num; i++) {
        scanf("%d", &value);

        if(i == 0) seqstart = value;
        if(i == num - 1) seqend = value;

       // printf("seqstart: %d, seqend: %d\n", seqstart, seqend);

        if(value < 0) {
            if(tmpsum == 0) {
                first_zero_flag = false;
                continue;
            }
            if(tmpsum+value < 0) {
                first_zero_flag = false;
                tmpsum = 0;
                ex_first = *first;
                continue;
            }
        }

        all_negative_flag = false;

        if((!first_zero_flag) && (value == 0)&&(tmpsum == 0)) {
           first_zero_flag = true; 
           *first = value; 
        }

        if((!first_zero_flag) &&(tmpsum == 0)) {
            *first = value; 
        }

        tmpsum += value;
        if(tmpsum > maxsum) {
            maxsum = tmpsum; 
            *last = value;
        }else {
            if(value > 0) {
                *first = ex_first;
            }
        }

        
    }
    if((maxsum == 0)&&(all_negative_flag)) {
        *first = seqstart;
        *last = seqend;
        //printf("start: %d, end: %d\n", *first, *last);
    }
    return maxsum;
}

int main(void)
{
    int num = 0;
    int maxsubseqsum = 0;
    int first=0, last=0;

    scanf("%d", &num);
    if(num <= 0) return -1;

    maxsubseqsum= get_max_sub_sequence_sum_and_border(num, &first, &last);
    printf("%d %d %d\n", maxsubseqsum, first, last);

    return 0;
}
