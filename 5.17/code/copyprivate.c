#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i;
    #pragma omp parallel num_threads(2) private(i)
    {
        #pragma omp single copyprivate(i)
        {
            printf("%d\n", omp_get_thread_num());
            scanf("%d", &i);
        }
        i += omp_get_thread_num();
        printf("%d\n", i);
    }
    return 0;
}
