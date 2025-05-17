#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i = 10, j = 20, n = 30, nT;
    printf("线程数:");
    scanf("%d", &nT);
    #pragma omp parallel num_threads(nT)
    {
        printf("output1:  %d  %d  %d\n", i, j, n);
        #pragma omp barrier
        #pragma omp sections private(i) firstprivate(n) lastprivate(j)
        {
            #pragma omp section //本行可删除
            {
                i = 11; n += 10; j = n;
                printf("n = %d  thread = %d\n", n, omp_get_thread_num());
            }
            #pragma omp section
            {
                i = 12; n += 20; j = n;
                printf("n = %d  thread = %d\n", n, omp_get_thread_num());
            }
            #pragma omp section
            {
                i = 12; n += 40; j = n;
                printf("n = %d  thread = %d\n", n, omp_get_thread_num());
            }
        }
        printf("output2:  %d  %d  %d\n", i, j, n);
    }
    printf("output3:  %d  %d  %d\n", i, j, n);
    return 0;
}
