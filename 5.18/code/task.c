#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i = 0, a[5];
    #pragma omp parallel num_threads(8)
    #pragma omp single
    while (i < 5){
        i++;
        #pragma omp task firstprivate(i) untied
        {
            a[i - 1] = i;
            printf("%d  %d\n", i, omp_get_thread_num());
        }
    }
    for ( i = 0; i < 5; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\n");
    return 0;
}
