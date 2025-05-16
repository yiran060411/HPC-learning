#include <stdio.h>
#include <omp.h>
int main(){
    int i = 1;
    #pragma omp parallel num_threads(2) reduction(+:i)
    {
        printf("初值  %d\n", i);
        #pragma omp barrier
        i += omp_get_thread_num() + 1;
        #pragma omp barrier
        printf("%d\n", i);
    }
    printf("终值  %d\n", i);
    return 0;
}
