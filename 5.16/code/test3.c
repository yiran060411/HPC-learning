#include <stdio.h>
#include <omp.h>
void output(){
    #pragma omp parallel num_threads(2)
    printf("inner: region 2\n");//嵌套并行
    return;
}
int main(){
    int n = 3;
    printf("begin...\n");//代码块1串行
    printf("n = %d\n", n);
    omp_set_max_active_levels(n);
    #pragma omp parallel num_threads(3)
    {
        //代码块2并行
        printf("outer: region 1\n");
        output();
        #pragma omp parallel num_threads(2)
        printf("inner: region 3\n");
    }
    printf("end...\n");
    
    return 0;
}
