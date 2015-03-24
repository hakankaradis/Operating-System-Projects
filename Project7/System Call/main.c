#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include "time_functions.h"
//define the system call
#define __NR_call_func 310
struct Memory{
	int A;
	int B;
	int C;
};
#define N 100000

void first_version();
void second_version();
int main(int argc, char *argv[])
{
	first_version();	
	second_version();
	return 0;	
}

void first_version(){
	int ret, i, j, A[N], B[N], C[N];

        //Loop to demonstrate the system call
        for(i=0;i<N;++i){
                A[i] = i+2;
                B[i] = i+4;
        }
	start_timing();
        syscall(__NR_call_func, 0);

        for (j=0; j<N; ++j) {
                C[j] = A[j] * B[j];

        }
	stop_timing();
        ret = syscall(__NR_call_func, 1);
        //Error checking
        if(ret == -1)
        {
                perror("System call failed\n");
                exit(1);
        }
	printf("first version results\n");
        printf("CPU time is %10.3f seconds\n",get_CPU_time_diff());
        printf("%ld \n", ret);
}
void second_version(){
        struct Memory *mem = malloc(sizeof(struct Memory)*N);
        int ret, i, j;

        //Loop to demonstrate the system call
        for(i=0;i<N;++i){
                mem[i].A = i+2;
                mem[i].B = i+4;
        }
	start_timing();
        syscall(__NR_call_func, 0);

        for (j=0; j<N; ++j) {
                mem[i].C = mem[j].A * mem[j].B;

        }
	stop_timing();
        ret = syscall(__NR_call_func, 1);
        //Error checking
        if(ret == -1)
        {
                perror("System call failed\n");
                exit(1);
        }
	printf("second version results\n");
        printf("CPU time is %10.3f seconds\n",get_CPU_time_diff());
	printf("%ld \n",ret);
}
