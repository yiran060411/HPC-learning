#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i, th;
    #pragma omp parallel num_threads(2) private(i,th)
    {
        th = omp_get_thread_num();
        #pragma omp critical
        {
            printf("线程%d正在读取:\n", th);
            scanf("%d", &i);
        }
        #pragma omp barrier
        #pragma omp critical (output)
        printf("线程%d中i的值为%d:\n", th, i);
    }
    return 0;
}
