#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i, j;
    #pragma omp parallel num_threads(6)
    {
        #pragma omp for collapse(2)
        for ( i = 1; i < 4; i++)
        {
            for ( j = 1; j < 3; j++)
            {
                printf("%d  %d  %d\n", i, j, omp_get_thread_num());
            }
            
        }
        
    }
    return 0;
}
