#include <stdio.h>
#include <omp.h>
int main(){
    int i = 1, j = 2, k = 3;
    #pragma omp parallel num_threads(2) \
    private(i) firstprivate(j) shared(k)
    {
        printf("初值  %d  %d  %d\n", i, j, k);
        #pragma omp barrier //线程同步
        i = omp_get_thread_num();
        j += omp_get_thread_num();
        if (omp_get_thread_num() == 0) k = 0;
        #pragma omp barrier
        printf("%d  %d  %d\n", i, j, k);
    }
    printf("终值  %d  %d  %d\n", i, j, k);
    return 0;
}
