#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int i, a[5], b[5];
    for ( i = 0; i < 5; i++)
    {
        a[i] = -1;
        b[i] = 0;
    }
    a[0] = 1;
    #pragma omp parallel
    {
        #pragma omp for ordered 
        for ( i = 0; i < 5; i++)
        {
            a[i] = i + 1;
            #pragma omp ordered
            b[i] = a[i - 1];
        }
        for ( i = 0; i < 5; i++)
        {
            printf("%d  %d\n", a[i], b[i]);
        }
        
    }
    return 0;
}
