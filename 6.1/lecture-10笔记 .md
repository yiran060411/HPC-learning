# lecture-10笔记

# 并行编程实践——高斯消元法与MPI实现详解

## 理论概念

### 高斯消元法基础

高斯消元法是一种解线性方程组的经典算法，也可用于求矩阵的秩和逆矩阵。它的基本思想是通过**初等行变换**将线性方程组的增广矩阵转化为行阶梯形矩阵（上三角矩阵），然后通过回代求解未知数。具体来说，高斯消元分为两个阶段：

- **消元阶段**：从第一行开始，逐行将当前行下方各行的对应主元列元素消为零。例如，对于第i行，找到主元（通常选当前列中绝对值最大的元素以增加数值稳定性），然后用该行的倍数消去下方所有行在第i列的元素。这一步通过行交换（Row Swap）、某行乘以非零常数、某行加上另一行的倍数等操作实现。
- **回代阶段**：将矩阵转化为上三角形式后，从最后一行开始向上回代求解每个未知数。例如，最后一行可直接解出最后一个未知数，然后将其代入倒数第二行解倒数第二个未知数，依此类推。

通过上述步骤，原方程组被转化为一个等价的上三角方程组，从而容易求解。高斯消元法的时间复杂度为O(n³)，对于大规模矩阵来说计算量很大，因此并行化是提高效率的重要途径。

> 注：初等行变换包括：交换两行位置、用非零数乘某一行、用某一行加上另一行的倍数。这些操作不会改变方程组的解。
> 

### 并行编程基础

并行编程旨在利用多处理器或多核计算资源，将任务分解并同时执行，以加快计算速度。常见的并行编程模型包括共享内存并行和分布式内存并行。在共享内存系统中，多个线程可以直接访问同一块内存，通常使用OpenMP等API通过线程实现并行；而在分布式内存系统中，各进程拥有独立内存，进程间通过消息传递交换数据，MPI（Message Passing Interface）是分布式并行编程的事实标准。

MPI是一种跨平台的消息传递协议，用于编写并行程序，支持点对点通信和广播等操作。MPI程序通常采用**单程序多数据（SPMD）**模式，即所有进程运行同一个程序，但根据进程ID的不同执行不同的任务或处理不同的数据。MPI提供了丰富的通信函数，例如：

- **点对点通信**：`MPI_Send`和`MPI_Recv`用于进程间发送和接收消息；`MPI_Isend`和`MPI_Irecv`等非阻塞通信可以与计算重叠，提高效率。
- **集合通信**：`MPI_Bcast`用于广播消息（一个进程发送数据给所有其他进程）；`MPI_Scatter`将数据从一个进程分发给所有进程；`MPI_Gather`和`MPI_Allgather`用于将各进程的数据收集到一个或所有进程；`MPI_Reduce`和`MPI_Allreduce`则通过指定运算符（如求和、求最大值）将各进程的数据进行归约。
- **同步操作**：`MPI_Barrier`用于进程同步，确保所有进程都达到某一点后才继续执行。

## 高斯消元法的MPI并行实现

### 数据分布策略

在并行高斯消元中，数据分布策略直接影响通信量和负载平衡，进而影响整体性能。常见的数据分布方式有：

1. **一维行块分布**：将矩阵按行分成大致相等的块，每个进程处理一块连续的行。这种分布简单易实现，但在列方向上通信开销较大。
2. **一维列块分布**：将矩阵按列分成大致相等的块，每个进程处理一块连续的列。这种分布在行方向上通信开销较大。
3. **二维块分布**：将矩阵分成一个二维网格，每个进程负责一个子块。这种分布在行和列方向上都有较好的通信性能，适合大规模问题，但实现较复杂。
4. **循环分布**：矩阵的行（或列）以循环方式分配给各进程，有助于实现更好的负载平衡，但可能增加通信复杂性。

> 注：上图展示了不同的数据分布策略。左侧是一维分布（按行或按列），右侧是二维分布（块状或循环）。不同颜色代表分配给不同进程的数据块。
> 

### 基本实现步骤

下面是基于MPI实现并行高斯消元的基本步骤：

1. **初始化**：使用`MPI_Init`初始化MPI环境，获取进程总数和当前进程ID。
2. **数据分布**：根据选定的分布策略，将矩阵数据分配给各个进程。
3. **主循环**：对每一列进行消元操作：
    - 选主元（部分主元选择）：找出当前列中绝对值最大的元素作为主元
    - 广播主元所在行：主元所在进程使用`MPI_Bcast`将该行广播给所有进程
    - 消元：各进程对自己负责的行进行消元操作
    - 前进到下一列
4. **回代**：从最后一个未知数开始，逐个求解所有未知数。
5. **收集结果**：使用`MPI_Gather`或类似函数将各进程的结果收集到主进程。
6. **结束**：使用`MPI_Finalize`清理MPI环境并退出。

### 简化代码示例

下面是一个简化的MPI并行高斯消元示例代码：

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
    int rank, size, n, local_n;
    double *A, *local_A, *pivot_row;

    // 初始化MPI环境
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 假设矩阵大小为n×n，主进程读入数据
    if (rank == 0) {
        printf("输入矩阵大小n: ");
        scanf("%d", &n);
        A = (double*)malloc(n * n * sizeof(double));
        // 假设这里读入了矩阵A的数据
    }

    // 广播矩阵大小给所有进程
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 计算每个进程处理的行数（简化处理，假设能整除）
    local_n = n / size;

    // 各进程分配内存
    local_A = (double*)malloc(local_n * n * sizeof(double));
    pivot_row = (double*)malloc(n * sizeof(double));

    // 主进程分发数据（简化的一维行块分布）
    MPI_Scatter(A, local_n * n, MPI_DOUBLE, local_A, local_n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // 消元过程
    for (int k = 0; k < n; k++) {
        int proc_with_pivot = k / local_n;  // 拥有主元的进程

        // 找出主元所在行并广播
        if (rank == proc_with_pivot) {
            int local_row = k % local_n;
            for (int j = 0; j < n; j++)
                pivot_row[j] = local_A[local_row * n + j];
        }
        MPI_Bcast(pivot_row, n, MPI_DOUBLE, proc_with_pivot, MPI_COMM_WORLD);

        // 计算本地消元（跳过主元所在行）
        for (int i = 0; i < local_n; i++) {
            if (rank == proc_with_pivot && i == k % local_n)
                continue;

            // 只处理k+1及以后的行
            if (rank * local_n + i > k) {
                double factor = local_A[i * n + k] / pivot_row[k];
                for (int j = k; j < n; j++)
                    local_A[i * n + j] -= factor * pivot_row[j];
            }
        }
    }

    // 收集结果（简化，实际应用中还要回代求解）
    MPI_Gather(local_A, local_n * n, MPI_DOUBLE, A, local_n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // 这里应该进行回代求解，省略...
        free(A);
    }

    free(local_A);
    free(pivot_row);
    MPI_Finalize();
    return 0;
}
```