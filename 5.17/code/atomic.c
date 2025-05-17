#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i = 0;
    #pragma omp parallel num_threads(10000)
    {
        #pragma omp atomic
        i++;
    }
    printf("%d\n", i);
    return 0;
}
