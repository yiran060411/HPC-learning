#include <stdio.h>
#include <omp.h>

int main() {
    // 获取CPU核心数
    printf("CPU核心数: %d\n", omp_get_num_procs());

    // 方式1：默认线程数（等于CPU核心数）
    #pragma omp parallel

    printf("region 1  %d  %d\n", omp_get_thread_num(), omp_get_num_threads());
    printf("----------\n");

    // 方式2：使用API设置线程数
    omp_set_num_threads(2);
    #pragma omp parallel
    printf("region 2  %d  %d\n", omp_get_thread_num(), omp_get_num_threads());
    printf("----------\n");

    #pragma omp parallel
    printf("region 3  %d  %d\n", omp_get_thread_num(), omp_get_num_threads());
    printf("----------\n");
    // 方式3：通过num_threads子句指定
    #pragma omp parallel num_threads(3)
    printf("region 4  %d  %d\n", omp_get_thread_num(), omp_get_num_threads());
    printf("end...\n");

    return 0;
}
