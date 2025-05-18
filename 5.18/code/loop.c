#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int j = 2, a[5];
    #pragma omp parallel
    {
        #pragma omp loop
        {
            for (int i = 0; i < 5; i++)
            {
                a[i] = (i + 1) * j;
                printf("%d  %d\n", i + 1, omp_get_thread_num());
            }
            
        }
    }
    for (int i = 0; i < 5; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\n");
    return 0;
} 
