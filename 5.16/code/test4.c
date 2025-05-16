#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <omp.h>
int main(){
    int nT, nP;
    printf("线程数:");
    scanf("%d", &nT);//读取线程数
    nP = omp_get_num_procs();//CPU核心数
    printf("CPU核心数:%d\n", nP);
    #pragma omp parallel num_threads(nT) if(nP>nT)
    printf("Hello\n");
    return 0;
}
