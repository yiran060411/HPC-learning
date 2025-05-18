#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i;
    #pragma omp parallel
    #pragma omp single
    {
        printf("%d\n", omp_get_thread_num());
        #pragma omp taskloop untied
        for (int i = 0; i < 6; i++)
        {
            printf("%d  %d\n", i, omp_get_thread_num());
        }
    }
    return 0;
}
