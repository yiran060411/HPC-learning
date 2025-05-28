# lecture-4笔记

# **OpenMP并行编程基础与应用总结**

## **1. 引言**

并行计算是现代计算机科学中的重要领域，随着多核处理器和分布式系统的普及，掌握并行编程技术变得越来越重要。本笔记基于悉尼大学计算机科学课程的讲义，详细介绍OpenMP这一广泛使用的并行编程框架，适合初学者理解和学习。

## **2. 并行编程基础**

### **2.1 并行计算概述**

**并行计算(Parallel Computing)** 是一种同时使用多个计算资源解决计算问题的计算方式。并行计算的基本思想是将大型问题分解为多个可以同时求解的小问题。

与传统的**串行计算(Serial Computing)** 相比，并行计算具有以下优势：

- 解决更大规模的问题
- 节省计算时间
- 更有效地利用计算资源
- 克服物理限制（如单核心频率提升的瓶颈）

### **2.2 共享内存平台**

**共享内存平台(Shared Memory Platforms)** 是一种并行计算架构，其中多个处理器可以同时访问一个共同的内存空间。

主要特点：

- 所有处理器共享一个统一的**全局地址空间(Global Address Space)**
- 各个处理器也有自己的**私有变量(Private Variables)**
- 使用**线程(Threads)** 作为执行单位
- 处理器之间通过共享内存进行通信

*（注：本图为概念图，实际讲义中可能有相应的示意图）*

### **2.3 并行编程关键问题**

在共享内存编程中，需要特别注意以下问题：

### **2.3.1 竞争条件**

**竞争条件(Race Condition)** 是指多个线程同时访问和修改同一数据时，结果依赖于线程执行的精确时序。这通常会导致程序输出不确定性和错误。

例如，如果两个线程同时尝试递增一个共享变量，可能会导致更新丢失：

```c
c
// 线程1和线程2同时执行：
count = count + 1;// 可能导致只增加一次而非两次
```

### **2.3.2 同步机制**

为了解决竞争条件问题，需要使用**同步(Synchronization)** 机制，包括：

- **互斥锁(Mutex)**: 确保同一时间只有一个线程能访问共享资源
- **屏障(Barrier)**: 使线程在特定点等待，直到所有线程都到达该点
- **信号量(Semaphore)**: 控制对共享资源的访问数量
- **原子操作(Atomic Operations)**: 不可分割的操作，执行过程不会被中断

### **2.3.3 数据局部性**

**数据局部性(Data Locality)** 指程序访问内存的模式，良好的数据局部性可以提高缓存命中率，显著提升性能。

两种主要类型：

- **时间局部性(Temporal Locality)**: 最近访问过的数据很可能在不久的将来再次被访问
- **空间局部性(Spatial Locality)**: 访问某一内存位置附近的数据的概率较高

### **2.3.4 并行粒度**

**并行粒度(Parallelism Granularity)** 指任务分解的细化程度：

- **细粒度并行(Fine-grained Parallelism)**: 将问题分解为大量小任务
    - 优点：负载均衡好
    - 缺点：线程管理开销大，通信成本高
- **粗粒度并行(Coarse-grained Parallelism)**: 将问题分解为少量大任务
    - 优点：较低的线程管理开销
    - 缺点：可能导致负载不均衡

## **3. 矩阵计算的并行策略**

矩阵计算是并行编程中经典的应用场景，以下是常见的矩阵分割策略：

### **3.1 一维循环分割 (1D Cyclic)**

**一维循环分割** 按行（或列）以循环方式将矩阵分配给不同处理器：

- 处理器0取第0, P, 2P, ...行
- 处理器1取第1, P+1, 2P+1, ...行
- 依此类推

这种方法有助于实现负载均衡，特别是在行（或列）之间的计算量不均衡时。

### **3.2 一维块分割 (1D Block)**

**一维块分割** 将连续的行（或列）块分配给处理器：

- 处理器0取第0到(N/P-1)行
- 处理器1取第(N/P)到(2N/P-1)行
- 依此类推

这种方法优化了数据局部性，减少了跨处理器的通信需求。

### **3.3 一维块循环分割 (1D Block Cyclic)**

**一维块循环分割** 综合了前两种方法，先将矩阵分成块，然后以循环方式分配这些块：

- 处理器0取第0, P, 2P, ...块
- 处理器1取第1, P+1, 2P+1, ...块
- 依此类推

这种方法在保持良好数据局部性的同时，也实现了更好的负载均衡。

## **4. OpenMP框架介绍**

### **4.1 OpenMP概述**

**OpenMP(Open Multi-Processing)** 是一种支持跨平台共享内存多线程编程的API，它支持C, C++和Fortran语言。

主要特点：

- 基于**指令式编程(Directive-based programming)**，使用特殊的编译器指令（如#pragma）
- 遵循**fork-join并行模型**，主线程创建一组并行线程，执行完后再汇合
- 提供了丰富的**同步机制**
- 相比底层线程库，具有**更高的抽象级别**
- 支持**增量并行化**，可以逐步将顺序程序转换为并行程序

### **4.2 编译和执行OpenMP程序**

编译OpenMP程序需要特定的编译器选项：

- GCC编译器：`gcc -fopenmp program.c -o program`
- Intel编译器：`icc -qopenmp program.c -o program`
- MSVC编译器：`cl /openmp program.c`

运行时，可以通过环境变量控制线程数：

```bash
bash
export OMP_NUM_THREADS=4# 在Linux/Mac上设置使用4个线程
./program# 运行程序
```

## **5. OpenMP核心构造**

### **5.1 并行区域 (Parallel Regions)**

**并行区域** 是OpenMP中最基本的构造，使用`#pragma omp parallel`指令创建：

```c
c
#pragma omp parallel
{
// 这段代码将被多个线程并行执行printf("Hello from thread %d\n", omp_get_thread_num());
}
```

执行时：

1. 主线程遇到并行区域时，创建一个线程团队
2. 每个线程执行相同的代码块
3. 在并行区域结束时有一个隐式的屏障，所有线程在此同步

### **5.2 工作共享构造 (Work-Sharing Constructs)**

工作共享构造用于在线程之间分配工作：

### **5.2.1 for循环并行化**

```c
c
#pragma omp parallel
{
    #pragma omp for
    for(int i=0; i<N; i++) {
// 循环迭代会被分配给不同线程array[i] = function(i);
    }
}
```

也可以合并为一条指令：

```c
c
#pragma omp parallel for
for(int i=0; i<N; i++) {
    array[i] = function(i);
}
```

### **5.2.2 sections并行化**

用于执行不同的代码段：

```c
c
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section
        {
// 代码段1，由一个线程执行
            task1();
        }

        #pragma omp section
        {
// 代码段2，由另一个线程执行
            task2();
        }
    }
}
```

### **5.2.3 单一执行 (single)**

确保代码块只被一个线程执行：

```c
c
#pragma omp parallel
{
// 并行代码

    #pragma omp single
    {
// 这段代码只由一个线程执行printf("This is executed by only one thread\n");
    }

// 继续并行执行
}
```

### **5.3 数据共享属性 (Data-Sharing Attributes)**

控制变量在线程间的可见性和访问权限：

- **shared**: 变量在所有线程间共享，所有线程访问同一内存位置
- **private**: 每个线程有自己的私有副本
- **firstprivate**: 私有副本，但用共享变量的值初始化
- **lastprivate**: 私有变量，并在并行区域结束时将值赋给共享变量
- **reduction**: 执行归约操作（如求和、求最大值等）

```c
c
int sum = 0;
#pragma omp parallel for reduction(+:sum)
for(int i=0; i<N; i++) {
    sum += array[i];// 安全地累加，避免竞争条件
}
```

### **5.4 同步构造 (Synchronization Constructs)**

### **5.4.1 屏障 (barrier)**

所有线程都必须到达屏障才能继续执行：

```c
c
#pragma omp parallel
{
// 第一阶段工作

    #pragma omp barrier

// 第二阶段工作，所有线程完成第一阶段后才开始
}
```

### **5.4.2 临界区 (critical)**

确保同一时间只有一个线程执行代码块：

```c
c
#pragma omp parallel
{
// 并行代码

    #pragma omp critical
    {
// 一次只有一个线程可以执行这里的代码
        shared_resource += local_result;
    }
}
```

### **5.4.3 原子操作 (atomic)**

轻量级同步，只保护单个内存访问：

```c
c
#pragma omp parallel
{
    #pragma omp atomic
    counter += 1;// 原子递增，避免竞争条件
}
```

## **6. OpenMP编程实例**

### **6.1 Hello World示例**

最基本的OpenMP程序：

```c
c
#include <stdio.h>#include <omp.h>int main() {
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();

        printf("Hello World from thread %d of %d\n",
               thread_id, total_threads);
    }

    return 0;
}
```

### **6.2 向量加法**

并行实现两个向量相加：

```c
c
#include <stdio.h>#include <omp.h>void vector_add(float *a, float *b, float *c, int n) {
    #pragma omp parallel for
    for(int i=0; i<n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    const int N = 100000;
    float a[N], b[N], c[N];

// 初始化向量for(int i=0; i<N; i++) {
        a[i] = i * 1.0f;
        b[i] = i * 2.0f;
    }

    double start_time = omp_get_wtime();
    vector_add(a, b, c, N);
    double end_time = omp_get_wtime();

    printf("向量加法用时: %f 秒\n", end_time - start_time);

// 验证结果for(int i=0; i<5; i++) {
        printf("c[%d] = %f\n", i, c[i]);
    }

    return 0;
}
```

### **6.3 矩阵-向量乘法**

并行实现矩阵与向量相乘：

```c
c
#include <stdio.h>#include <omp.h>void matrix_vector_mult(float **A, float *x, float *y, int rows, int cols) {
    #pragma omp parallel for
    for(int i=0; i<rows; i++) {
        y[i] = 0.0f;
        for(int j=0; j<cols; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}
```

## **7. 性能评估与优化**

### **7.1 性能测量**

OpenMP提供了内置函数来测量程序执行时间：

```c
c
double start_time = omp_get_wtime();// 获取当前时间（秒）// 执行要测量的代码double end_time = omp_get_wtime();
printf("执行时间: %f 秒\n", end_time - start_time);
```

### **7.2 加速比与效率**

**加速比(Speedup)** 是串行执行时间与并行执行时间的比值：

```

Speedup = T_serial / T_parallel
```

**并行效率(Efficiency)** 是加速比除以使用的处理器数量：

```

Efficiency = Speedup / Number_of_processors
```

理想情况下：

- 线性加速比：使用p个处理器能获得p倍加速比
- 效率为1.0（或100%）

实际中由于并行开销、负载不均衡等因素，效率通常小于1.0。

### **7.3 常见优化策略**

1. **减少线程管理开销**：
    - 避免创建过多小的并行区域
    - 可以合并多个小并行区域为一个大的并行区域
2. **优化数据访问**：
    - 改善数据局部性，减少缓存未命中
    - 减少伪共享（false sharing）
    - 使用适当的内存对齐
3. **负载均衡**：
    - 使用动态调度（`schedule(dynamic)`）处理不均衡工作负载
    - 使用导引（guided）调度提高局部性的同时保持负载均衡
4. **减少同步开销**：
    - 避免不必要的同步点
    - 使用`nowait`跳过不需要的隐式屏障
    - 选择合适的粒度，降低同步次数

## **8. 实验练习**

### **8.1 基本并行化练习**

1. 使用OpenMP并行化计算π的值：

```c
c
#include <stdio.h>#include <omp.h>int main() {
    const long num_steps = 100000000;
    const double step = 1.0 / num_steps;
    double sum = 0.0;

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        double local_sum = 0.0;

        #pragma omp for
        for(long i=0; i<num_steps; i++) {
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x*x);
        }

        #pragma omp critical
        sum += local_sum;
    }

    double pi = sum * step;
    double end_time = omp_get_wtime();

    printf("π估计值 = %.15f\n", pi);
    printf("计算用时: %f 秒\n", end_time - start_time);

    return 0;
}
```

可以改进为使用reduction，避免临界区：

```c
c
#pragma omp parallel for reduction(+:sum)
for(long i=0; i<num_steps; i++) {
    double x = (i + 0.5) * step;
    sum += 4.0 / (1.0 + x*x);
}
```

### **8.2 性能评估练习**

比较不同调度策略对性能的影响：

```c
c
#include <stdio.h>#include <omp.h>void test_schedule(const char* type, int chunk_size, int array_size) {
    double start, end;
    int i;

    printf("调度策略: %s, 块大小: %d\n", type, chunk_size);

    start = omp_get_wtime();

    if(strcmp(type, "static") == 0) {
        #pragma omp parallel for schedule(static, chunk_size)
        for(i=0; i<array_size; i++) {
// 模拟计算工作double result = 0.0;
            for(int j=0; j<i%1000; j++) {
                result += sin(j) * cos(j);
            }
        }
    } else if(strcmp(type, "dynamic") == 0) {
        #pragma omp parallel for schedule(dynamic, chunk_size)
        for(i=0; i<array_size; i++) {
// 模拟计算工作double result = 0.0;
            for(int j=0; j<i%1000; j++) {
                result += sin(j) * cos(j);
            }
        }
    } else if(strcmp(type, "guided") == 0) {
        #pragma omp parallel for schedule(guided, chunk_size)
        for(i=0; i<array_size; i++) {
// 模拟计算工作double result = 0.0;
            for(int j=0; j<i%1000; j++) {
                result += sin(j) * cos(j);
            }
        }
    }

    end = omp_get_wtime();
    printf("执行时间: %f 秒\n\n", end - start);
}

int main() {
    const int ARRAY_SIZE = 10000;

    test_schedule("static", 1, ARRAY_SIZE);
    test_schedule("static", 100, ARRAY_SIZE);
    test_schedule("dynamic", 1, ARRAY_SIZE);
    test_schedule("dynamic", 100, ARRAY_SIZE);
    test_schedule("guided", 1, ARRAY_SIZE);

    return 0;
}
```

## **9. 专业术语表**

### **并行计算基础术语**

- **并行计算(Parallel Computing)**: 同时使用多个计算资源解决计算问题的计算方式。
- **串行计算(Serial Computing)**: 一次只执行一条指令的传统计算方式。
- **线程(Thread)**: 程序执行的最小单位，一个进程可以包含多个线程，它们共享该进程的资源。
- **进程(Process)**: 正在执行的程序实例，拥有独立的内存空间。
- **并发(Concurrency)**: 在单处理器上通过时间片轮转同时处理多个任务的能力。
- **并行(Parallelism)**: 在多处理器上真正同时执行多个任务的能力。

### **内存架构术语**

- **共享内存(Shared Memory)**: 所有处理器可以直接访问的公共内存区域。
- **分布式内存(Distributed Memory)**: 每个处理器有自己的本地内存，不能直接访问其他处理器的内存。
- **全局地址空间(Global Address Space)**: 在共享内存系统中所有处理器可见的统一内存地址范围。
- **私有变量(Private Variables)**: 只能被单个线程访问的变量。
- **共享变量(Shared Variables)**: 可被多个线程同时访问的变量。

### **并行问题术语**

- **竞争条件(Race Condition)**: 程序行为依赖于多线程的相对执行时序的情况。
- **数据竞争(Data Race)**: 不同线程在没有同步的情况下同时访问同一位置，且至少有一个是写操作。
- **死锁(Deadlock)**: 两个或更多线程互相等待对方释放资源，导致永久阻塞。
- **活锁(Livelock)**: 线程不断地改变状态却无法向前推进的情况。
- **饥饿(Starvation)**: 线程无法获得所需资源导致无法继续执行的情况。

### **OpenMP特定术语**

- **指令(Directive)**: 在OpenMP中，用于表达并行意图的特殊注释或预处理器指令。
- **并行区域(Parallel Region)**: 由多个线程并行执行的代码块。
- **工作共享(Work-sharing)**: 在线程团队中分配工作的构造。
- **调度(Scheduling)**: 决定如何在线程间分配循环迭代的策略。
- **私有化(Privatization)**: 为每个线程创建变量的私有副本。
- **归约(Reduction)**: 将线程私有结果合并为单一结果的操作。
- **屏障(Barrier)**: 所有线程必须到达才能继续执行的同步点。
- **临界区(Critical Section)**: 一次只能由一个线程执行的代码区域。

### **并行性能术语**

- **加速比(Speedup)**: 串行执行时间与并行执行时间的比率。
- **阿姆达尔定律(Amdahl's Law)**: 预测并行程序加速比上限的公式，考虑了程序中不可并行部分的影响。
- **效率(Efficiency)**: 加速比与所用处理器数量的比值。
- **可扩展性(Scalability)**: 随着处理器数量增加，程序性能提升的能力。
- **负载均衡(Load Balancing)**: 在处理器之间平均分配工作的技术。
- **伪共享(False Sharing)**: 当多个线程访问同一缓存行中的不同数据时导致的性能下降现象。

## **10. 总结**

OpenMP是一个强大而易用的并行编程框架，特别适合共享内存系统。通过简单的指令，程序员可以逐步将顺序程序转换为并行程序，无需彻底重构代码。

本笔记涵盖了OpenMP的基础知识，包括：

- 并行编程的基本概念和挑战
- 共享内存平台特性
- OpenMP的核心构造和使用方法
- 实用的编程示例和优化策略

通过掌握OpenMP，可以有效地利用现代处理器的多核性能，提高程序执行效率。对于初学者来说，建议从简单例子开始，逐步深入理解并行编程的概念和技术，特别注重同步和数据共享的问题，避免常见的并行编程陷阱。