#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int get_max_sub_sequence_sum(int num)
{
    int i;
    int value;
    int tmpsum = 0;
    int maxsum = 0;

    for(i=0; i<num; i++) {
        scanf("%d", &value);

        if(value < 0) {
            if(tmpsum == 0) {
                continue;
            }
        }
        tmpsum += value;
        if(tmpsum > maxsum) {
            maxsum = tmpsum; 
        }
    }

    return maxsum;
}

int main(void)
{
    int num = 0;
    int maxsubseqsum = 0;

    scanf("%d", &num);
    if(num <= 0) return -1;

    maxsubseqsum= get_max_sub_sequence_sum(num);
    printf("%d\n", maxsubseqsum);

    return 0;
}
