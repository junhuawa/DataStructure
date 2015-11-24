#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>


#define DATA_LENGTH 300000

int sort_data(unsigned int *data, unsigned int len)
{
    unsigned int i = 0;
    unsigned int t, j = 0;

    for(i=0; i<len; i++) {
        for(j = i; j<len; j++) {
            if(data[i] > data[j]) {
                t = data[i];
                data[i]=data[j];
                data[j] = t;
            }
        }
    }
    return 0;
}
int fill_the_data_with_random_value(unsigned int *data, unsigned int len)
{ 
   unsigned int i=0;
   srand(time(NULL));
   for(i=0; i<len; i++) {
        data[i]= rand();
   }
   return 0;
}

int write_data_to_a_file(unsigned int *data, unsigned int len, int flag)
{
    int fd = 0;
    unsigned int i = 0;
    char buffer[32] = {'\0'};
    char filename[32] = {'\0'};
    
    if(flag == 0) {
        snprintf(filename, sizeof(filename), "data_before_sort.%d.txt", getpid());
    }else {
        snprintf(filename, sizeof(filename), "data_after_sort.%d.txt", getpid());
    }
    fd = open(filename, O_RDWR|O_CREAT, 0666);
    if(fd == -1) {
        printf("creat file error: %s\n", strerror(errno));
        return -1;
    }

    for(i=0; i<len; i++) {
    //    printf("%d\n", i);
        snprintf(buffer,sizeof(buffer), "%d\t%d\r\n", i, data[i]);
        write(fd, buffer, strlen(buffer));
        memset(buffer, 0, sizeof(buffer));
    }
    close(fd);
    return 0;
}
int main(void) 
{
   unsigned int data[DATA_LENGTH] = {'\0'}; 
   fill_the_data_with_random_value(data, sizeof(data)/sizeof(data[0]));
   write_data_to_a_file(data, sizeof(data)/sizeof(data[0]), 0);
   sort_data(data, sizeof(data)/sizeof(data[0]));
   write_data_to_a_file(data, sizeof(data)/sizeof(data[0]), 1);
   return 0;
}
