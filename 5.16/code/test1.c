#include <stdio.h>
#include <omp.h>
int main(){
    printf("begin...\n");//代码块1串行
#pragma omp parallel num_threads(2)
    printf("region 1\n");//代码块2并行
    printf("---------\n");//代码块3串行
#pragma omp parallel num_threads(3)
    printf("region 2\n");//代码块4并行
    printf("end...\n");//代码块5串行
}
