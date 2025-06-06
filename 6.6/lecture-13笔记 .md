# lecture-13笔记

# CUDA 矩阵乘法优化：从基础到分块技巧详解

本文内容主要基于 HUST-USYD Summer School on Parallel Programming Practice – Lecture 13 by Bing Bing Zhou (lecture13_2024) 编写，并结合公开资料对相关专业术语进行解释，旨在为初学者提供一份清晰易懂的学习笔记。

## 引言：为什么需要优化矩阵乘法？

矩阵乘法是科学计算、机器学习、图像处理等众多领域中的核心运算。由于其计算密集型的特点，如何高效地执行矩阵乘法至关重要。**图形处理器 (GPU)** 凭借其强大的并行处理能力，成为加速这类运算的理想选择。CUDA (Compute Unified Device Architecture) 作为 NVIDIA 推出的并行计算平台和编程模型，使得开发者能够利用 GPU 的强大算力。然而，仅仅将代码移植到 CUDA 上并不足以发挥 GPU 的全部潜能，性能优化是关键环节。

> 正如讲座资料 (lecture13_2024, p4) 指出的，一个基础的 GPU 矩阵乘法实现可能远未达到硬件的理论峰值性能，主要瓶颈在于内存访问效率。
> 

## 基础矩阵乘法及其性能瓶颈

### 基本思路

在基础的矩阵乘法 C = A * B 中，结果矩阵 C 的每一个元素 C[Row][Col] 是由矩阵 A 的第 Row 行与矩阵 B 的第 Col 列对应元素乘积之和计算得到的。在 CUDA 中，一个简单的实现方法是为结果矩阵 P (等同于 C) 的每一个元素分配一个线程来计算。每个线程负责计算 Pvalue，即 P[Row*Width+Col] 的值。

```

// __global__ 表示这是一个在 GPU 上执行的核函数
__global__ void MatrixMulKernel(float* M, float* N, float* P, int Width) {
  // 计算线程对应的输出矩阵 P 的行索引 (Row) 和列索引 (Col)
  int Row = blockIdx.y * blockDim.y + threadIdx.y;
  int Col = blockIdx.x * blockDim.x + threadIdx.x;

  // 确保线程计算的行列在矩阵范围内
  if ((Row < Width) && (Col < Width)) {
    float Pvalue = 0;
    // 每个线程计算结果矩阵 P 的一个元素
    // 遍历 A 矩阵的行和 B 矩阵的列
    for (int k = 0; k < Width; ++k) {
      Pvalue += M[Row * Width + k] * N[k * Width + Col]; // 访问全局内存
    }
    P[Row * Width + Col] = Pvalue; // 将结果写回全局内存
  }
}

```

代码片段来源: lecture13_2024, p3

### 性能瓶颈分析

上述基础实现中，每个线程在计算 Pvalue 的循环内会多次访问**全局内存 (Global Memory)**。全局内存虽然容量大，但访问延迟较高，带宽相对有限。频繁的全局内存访问是主要的性能瓶颈。

讲座资料 (lecture13_2024, p4) 提供了一个例子：

- 假设 GPU 峰值浮点计算速率为 1,500 GFLOPS。
- DRAM 带宽为 200 GB/s。
- 矩阵乘法中，每次浮点运算（乘加操作算两次运算，通常这里指一次乘法和一次加法涉及的数据）需要读取两个 float (8 bytes)。但 PDF 中简化为“One memory access (4 bytes) per floating-point operation”，可能指一次乘或一次加。如果按一次乘法需要两个 float (M 和 N 的元素)，一次加法一个 Pvalue，一次浮点乘加操作 (FMA) 涉及读取两个输入操作数。为了达到 1,500 GFLOPS，如果是 FMA (2 FLOPS/op)，则需要 1500/2 = 750 Giga-FMA-ops/sec。每个 FMA 需要加载两个 float (8 bytes)。因此理论上需要 750 * 8 = 6,000 GB/s 的内存带宽。
- 然而，可用内存带宽仅为 200 GB/s，这将实际性能限制在约 50 GFLOPS。
- 这意味着实际运行速率可能只有理论峰值速率的 3.33% (50/1500)。

为了提升性能，必须大幅减少对全局内存的访问次数，并有效利用更快的内存层级，如**共享内存 (Shared Memory)**。

## 核心优化方案：分块矩阵乘法 (Tiled Matrix Multiplication)

### 什么是分块矩阵乘法？

分块矩阵乘法是一种经典的优化策略，其核心思想是将大矩阵划分为若干个固定大小的子矩阵块 (Tiles)，然后针对这些子块进行计算。通过将频繁访问的子块数据加载到 GPU 上的快速**共享内存 (Shared Memory)** 中，可以显著减少对慢速全局内存的访问次数，从而提高数据复用率和计算效率。

基本步骤如下 (lecture13_2024, p7, p24)：

1. 将输入矩阵 M 和 N 的当前计算所需的子块 (tiles) 从全局内存加载到线程块 (Block) 内的共享内存 (ds_M, ds_N)。
2. 线程块内的所有线程**同步 (synchronize)**，确保数据加载完毕。
3. 线程块内的每个线程从共享内存中读取数据，计算其负责的部分乘积累加值 Pvalue。
4. 再次同步，确保所有线程完成了对当前子块数据的计算。
5. 重复以上步骤，处理所有子块对，直到完成整个输出矩阵元素 P[Row*Width+Col] 的计算。
6. 最后将 Pvalue 写回全局内存。

### 共享内存 (Shared Memory) 的妙用

共享内存是位于 GPU 每个 **SM (Streaming Multiprocessor)** 上的可编程存储器。它的访问速度远快于全局内存（讲座资料 lecture13_2024, p45 提到，共享内存比全局内存快约100倍），但容量较小。在分块矩阵乘法中，每个线程块将计算所需的 M 和 N 的子块加载到共享内存。这样，在计算子矩阵乘积时，原本需要多次从全局内存读取的数据，现在可以从快速的共享内存中多次读取，大大提高了访存效率。

### 线程同步 (`__syncthreads()`)

`__syncthreads()` 是一个块内同步原语。当调用此函数时，一个线程块 (Block) 内的所有线程都会在此处等待，直到块内的所有线程都到达这个同步点，然后它们才能继续执行。在分块算法中，同步至关重要 (lecture13_2024, p17)：

- **加载数据后同步：**确保所有线程都将子块数据从全局内存加载到共享内存完毕，然后才能开始使用这些共享内存中的数据进行计算。这防止了某些线程使用尚未被其他线程写入或更新完成的数据。
- **计算完成后（下一个加载前）同步：**在一个子块的计算阶段完成后，下一个子块数据加载到同一共享内存区域之前，需要同步。这确保所有线程对当前共享内存中数据的读取和使用已经完成，避免了数据被覆盖而引起的计算错误。

### 分块矩阵乘法核函数示例

以下是分块矩阵乘法的 CUDA Kernel 核心逻辑示例 (基于 lecture13_2024, p24)：

```

__global__ void TiledMatrixMulKernel(float* M, float* N, float* P, int Width) {
  // 定义共享内存，TILE_WIDTH 是子块的边长
  __shared__ float ds_M[TILE_WIDTH][TILE_WIDTH];
  __shared__ float ds_N[TILE_WIDTH][TILE_WIDTH];

  int bx = blockIdx.x;  // 线程块在 Grid 中的 X 索引
  int by = blockIdx.y;  // 线程块在 Grid 中的 Y 索引
  int tx = threadIdx.x; // 线程在 Block 中的 X 索引 (列)
  int ty = threadIdx.y; // 线程在 Block 中的 Y 索引 (行)

  // 计算当前线程负责的输出矩阵 P 元素的全局行和列
  int Row = by * blockDim.y + ty; // blockDim.y 通常等于 TILE_WIDTH
  int Col = bx * blockDim.x + tx; // blockDim.x 通常等于 TILE_WIDTH

  float Pvalue = 0; // 用于累加部分乘积

  // 循环处理所有子块对 (phases)
  // n 是矩阵的维度 (Width)，这里假设 Width 是 TILE_WIDTH 的倍数
  // p 是 phase 序号
  for (int p = 0; p < Width / TILE_WIDTH; ++p) {
    // 协同加载 M 的子块 ds_M 和 N 的子块 ds_N
    // 每个线程加载 ds_M 中的一个元素和 ds_N 中的一个元素
    // M[Row][p*TILE_WIDTH + tx] 映射到 ds_M[ty][tx]
    ds_M[ty][tx] = M[Row * Width + (p * TILE_WIDTH + tx)];
    // N[p*TILE_WIDTH + ty][Col] 映射到 ds_N[ty][tx]
    ds_N[ty][tx] = N[(p * TILE_WIDTH + ty) * Width + Col];

    __syncthreads(); // 同步，确保所有数据已加载到共享内存

    // 计算子块乘积并累加到 Pvalue
    // 每个线程计算 Pvalue 的一部分
    for (int i = 0; i < TILE_WIDTH; ++i) {
      Pvalue += ds_M[ty][i] * ds_N[i][tx];
    }

    __syncthreads(); // 同步，确保所有线程已完成当前子块的计算
  }

  // 将最终结果写回全局内存 P
  // (通常会有一个 if (Row < Width && Col < Width) 的边界检查)
  P[Row * Width + Col] = Pvalue;
}

```

注意：上述代码是原理性展示，实际应用中加载 `ds_M` 和 `ds_N` 的索引方式可能与图中所示加载模式有关，例如，`ds_N[ty][tx]`可能对应加载 `N` 矩阵中以列为主的块。讲义 p24 的代码显示 `ds_M[ty][tx] = M[Row*Width + p*TILE_WIDTH+tx];` 和 `ds_N[ty][tx] = N[(p*TILE_WIDTH+ty)*Width + Col];`。这里的 `ds_M` 加载是行优先的tile，`ds_N` 加载是列优先的tile元素到 `ds_N[ty][tx]`。为了后续 `ds_M[ty][i] * ds_N[i][tx]` 的正确内积，`ds_N` 元素在共享内存中的布局很重要。`ds_N[i][tx]` 意味着 `N` 的子块在共享内存中是转置存储，或者说 `ds_N[ty][tx]` 实际上加载的是 `N` 矩阵中第 `(p*TILE_WIDTH+ty)` 行，第 `Col` 列的元素，然后用于计算 `ds_N[i][tx]` (其中 `i` 对应 `ty`，`tx` 对应 `tx`)。 如果 `ds_N` 的加载是 `ds_N[tx][ty]` 来存储转置的tile，那么计算部分就是 `ds_M[ty][i] * ds_N[tx][i]` （或者类似形式）。 讲义 p24 的 `ds_N[ty][tx] = N[(p*TILE_WIDTH+ty)*Width + Col];` 和 `Pvalue += ds_M[ty][i] * ds_N[i][tx];` 是匹配的，它意味着 `ds_M` 对应 `P` 的子块的行，`ds_N` 对应 `P` 的子块的列。每个线程 `(ty,tx)` 计算 `P[Row][Col]`，它从 `ds_M[ty][0..TILE_WIDTH-1]` 和 `ds_N[0..TILE_WIDTH-1][tx]` 读取。所以加载 `ds_M[ty][tx]` 应该是 `M` 的 `Row` 行，`p*TILE_WIDTH+tx` 列的元素。加载 `ds_N[ty][tx]` 应该是 `N` 的 `p*TILE_WIDTH+ty` 行，`Col` 列的元素。 在计算时，`ds_M[ty][i]` 是 M 子块的第 ty 行第 i 列。`ds_N[i][tx]` 是 N 子块的第 i 行第 tx 列。这是标准的子矩阵乘法。

## 深入理解分块优化的细节

### 数据加载 (Loading a Tile)

在分块矩阵乘法中，线程块 (Block) 内的所有线程协同工作，从全局内存中将当前计算所需的 M 和 N 的子块加载到共享内存中。通常设计成每个线程负责加载共享内存中属于自己位置的一个或几个元素 (lecture13_2024, p7)。例如，对于一个 TILE_WIDTH x TILE_WIDTH 的子块，可以使用 TILE_WIDTH x TILE_WIDTH 个线程，每个线程 `(threadIdx.y, threadIdx.x)` (即 `(ty, tx)`) 负责加载 `ds_M[ty][tx]` 和 `ds_N[ty][tx]`。

### TILE_WIDTH (块大小) 的考量

`TILE_WIDTH` 的选择对性能有显著影响 (lecture13_2024, p27)：

- **线程数量：**一个线程块包含 `TILE_WIDTH * TILE_WIDTH` 个线程。较大的 `TILE_WIDTH` 意味着每个块的线程数更多，有助于更好地利用 SM 的计算资源，但也受限于 SM 的最大线程数。
- **共享内存占用：**每个块需要 `2 * TILE_WIDTH * TILE_WIDTH * sizeof(float)` 大小的共享内存。`TILE_WIDTH` 过大会导致共享内存不足，从而限制了 SM上可同时驻留的线程块数量，可能降低并行度。
- **计算访存比：**在每个阶段 (phase)，每个块从全局内存加载 `2 * TILE_WIDTH * TILE_WIDTH` 个浮点数，并执行 `TILE_WIDTH * TILE_WIDTH * (2 * TILE_WIDTH)` 次浮点运算 (乘法和加法)。计算访存比为 `(TILE_WIDTH*TILE_WIDTH * 2 * TILE_WIDTH) / (2 * TILE_WIDTH*TILE_WIDTH) = TILE_WIDTH`。更大的 `TILE_WIDTH` 意味着从共享内存中读取的每个数据被用于更多次计算，数据复用率更高。*(注: lecture13_2024, p27 的例子: TILE_WIDTH=16, 每个块执行 2*256=512 float loads, 256*(2*16)=8192 mul/add ops. Ops/load = 8192/512 = 16. TILE_WIDTH=32, 2*1024=2048 loads, 1024*(2*32)=65536 ops. Ops/load = 65536/2048 = 32. 这与 TILE_WIDTH 值一致。)*

例如，从讲义 (lecture13_2024, p27) 可知：

- **TILE_WIDTH = 16：**
    - 每个块 256 个线程 (16x16)。
    - 每次加载 2 * 256 = 512 个浮点数 (2KB，若 float 为 4B)。
    - 执行 256 * (2 * 16) = 8,192 次浮点运算。
    - 每个加载的浮点数支持 16 次浮点运算。
- **TILE_WIDTH = 32：**
    - 每个块 1024 个线程 (32x32)。
    - 每次加载 2 * 1024 = 2048 个浮点数 (8KB)。
    - 执行 1024 * (2 * 32) = 65,536 次浮点运算。
    - 每个加载的浮点数支持 32 次浮点运算。

### 共享内存与线程限制

选择 `TILE_WIDTH` 时还需考虑 SM 的硬件限制 (lecture13_2024, p28)：

- **SM 共享内存容量：**例如，如果一个 SM 有 16KB 共享内存。
    - `TILE_WIDTH = 16` (256 线程/块): 每个块使用 2 * 256 * 4 Bytes = 2KB 共享内存。理论上一个 SM 可以容纳 16KB / 2KB = 8 个这样的块。
    - `TILE_WIDTH = 32` (1024 线程/块): 每个块使用 2 * 1024 * 4 Bytes = 8KB 共享内存。理论上一个 SM 可以容纳 16KB / 8KB = 2 个这样的块。
- **SM 最大线程数：**例如，Fermi 架构的 SM 最大支持 1536 个线程。
    - 对于 `TILE_WIDTH = 16` (256 线程/块): 8 个块 * 256 线程/块 = 2048 线程，超过了 1536 的限制。因此，实际 SM 只能同时运行 1536 / 256 = 6 个块。
    - 对于 `TILE_WIDTH = 32` (1024 线程/块): 2 个块 * 1024 线程/块 = 2048 线程，也超过了1536的限制。因此，实际 SM 只能同时运行 1536 / 1024 = 1 个块（向下取整）。

因此，需要权衡共享内存占用、线程数量限制以及计算访存比，以达到最佳的 SM 利用率和并行度。

### 线程块粒度 (Block Granularity) 的选择

线程块的维度（例如，8x8, 16x16, 32x32 线程）对 SM 的资源利用率有直接影响。讲义 (lecture13_2024, p29) 以 Fermi 架构为例（每个 SM 最多 1536 线程，最多 8 个线程块）进行了分析：

- **8x8 (64 线程/块)：**
    - 每个 SM 最多可以容纳 8 个块 (受限于SM的块数量上限)。
    - 总线程数：8 块 * 64 线程/块 = 512 线程/SM。这远未达到 SM 的 1536 线程容量。
- **16x16 (256 线程/块)：**
    - 每个 SM 最多可以容纳 1536 / 256 = 6 个块。
    - 总线程数：6 块 * 256 线程/块 = 1536 线程/SM。这可以充分利用 SM 的线程容量，除非有其他资源（如共享内存）限制。
- **32x32 (1024 线程/块)：**
    - 每个 SM 最多可以容纳 1536 / 1024 = 1 个块 (向下取整)。
    - 总线程数：1 块 * 1024 线程/块 = 1024 线程/SM。这只利用了 SM 约 2/3 (1024/1536) 的线程容量。

从中可以看出，对于 Fermi 架构，16x16 的线程块似乎是一个较好的选择，因为它能更好地平衡线程数量和块数量的限制，以期达到较高的 SM 利用率。

## 处理任意大小的矩阵 (Handling Arbitrary Size)

### 问题描述

前面讨论的分块矩阵乘法 Kernel 通常假设矩阵的维度 (Width) 是 `TILE_WIDTH` 的整数倍。然而，实际应用中矩阵大小是任意的 (lecture13_2024, p30)。简单地通过填充 (padding) 额外的零元素将矩阵扩展到 `TILE_WIDTH` 的倍数，会浪费存储空间和数据传输时间。

### “简单”解决方案：边界检查

一个更有效的方法是在 Kernel内部进行边界检查 (lecture13_2024, p36-41)：

1. **加载输入数据时：**当线程尝试从全局内存加载 M 或 N 的元素到共享内存时，需要检查计算出的全局内存索引是否在原矩阵的有效范围内。
    - 如果索引有效，则加载实际数据。
    - 如果索引无效（越界），则向共享内存的对应位置写入 0。这样做的好处是，无效数据参与乘加运算时不会影响最终结果 (乘以0)。
2. **存储输出数据时：**在将最终计算结果 Pvalue 写回全局内存 P 之前，需要检查当前线程负责的 P 元素的行 (Row) 和列 (Col) 索引是否在输出矩阵 P 的有效范围内。只有在范围内才进行写操作。

重要的是，负责计算有效 P 元素的线程，在加载其所需的输入数据块时，仍可能尝试访问超出 M 或 N 边界的元素。同样，一些线程可能不负责计算有效的 P 元素（因为其 (Row, Col) 超出 P 的边界），但它们仍需参与加载其线程块共享内存中有效的 M 或 N 的数据块。

边界检查示例代码片段 (lecture13_2024, p40)：

```

// 在循环加载子块 (phase p) 内部
// ...
// 加载 M 的子块元素，带边界检查
if (Row < Width && (p * TILE_WIDTH + tx) < Width) {
    ds_M[ty][tx] = M[Row * Width + (p * TILE_WIDTH + tx)];
} else {
    ds_M[ty][tx] = 0.0f;
}

// 加载 N 的子块元素，带边界检查
if ((p * TILE_WIDTH + ty) < Width && Col < Width) {
    ds_N[ty][tx] = N[(p * TILE_WIDTH + ty) * Width + Col];
} else {
    ds_N[ty][tx] = 0.0f;
}

__syncthreads();

// ... 计算 Pvalue ...

__syncthreads();
// ... 结束 phase p 循环 ...

// 存储最终结果 Pvalue，带边界检查
if (Row < Width && Col < Width) {
    P[Row * Width + Col] = Pvalue;
}

```

循环的次数也需要调整为 `(Width - 1) / TILE_WIDTH + 1` 以确保覆盖所有数据。

## CUDA 编程模型核心概念回顾

为了更好地理解和编写 CUDA 程序，回顾一些核心概念是非常有帮助的 (lecture13_2024, p43-47)。

### 典型 CUDA 程序结构

1. **定义 Kernel 函数：**使用 `__global__`限定符定义的函数，它会在 GPU 上由大量线程并行执行。
2. **主机端 (CPU) 主要流程：**
    - 在 GPU 设备上分配内存：`cudaMalloc()`。
    - 将数据从主机内存拷贝到 GPU 设备内存：`cudaMemcpy(..., cudaMemcpyHostToDevice)`。
    - 调用 Kernel 函数：`kernelName<<>>(args...)`，其中 `DimGrid` 指定了 Grid 的维度和大小（线程块的数量），`DimBlock` 指定了每个 Block 的维度和大小（每个块内的线程数量）。
    - 将计算结果从 GPU 设备内存拷贝回主机内存：`cudaMemcpy(..., cudaMemcpyDeviceToHost)`。
    - 释放 GPU 设备内存：`cudaFree()`。
    - (可选) 与 CPU 计算的“黄金”结果进行比较以验证正确性。

### 线程组织：Grid, Block, Thread, Warp

- **Thread (线程)：**CUDA 中最基本的执行单元。每个线程独立执行 Kernel 代码。通过内置变量 `threadIdx`（线程在块内的索引）、`blockDim`（块的维度）可以获取线程的局部ID。
- **Block (线程块)：**一组线程的集合，可以是一维、二维或三维的。同一个块内的线程可以共享数据（通过共享内存）并且可以进行同步 (`__syncthreads()`)。块内的线程被调度到同一个 SM 上执行。通过内置变量 `blockIdx`（块在网格内的索引）、`gridDim`（网格的维度）可以获取块的全局ID。
- **Grid (线程网格)：**一组线程块的集合，同样可以是一维或二维（CUDA中Grid可以是三维，但通常用于更复杂的场景）。一个 Kernel 函数的调用会启动一个 Grid 中的所有线程。不同块之间的线程通常不能直接通信或同步（除非使用较新的 CUDA 特性如 Cooperative Groups 或原子操作）。
- **Warp (线程束)：**SM上线程调度的基本单位。一个 Warp 通常由 32 个连续的线程组成。Warp 内的线程以 **SIMD (Single Instruction, Multiple Data)** 方式执行，即它们同时执行相同的指令，但处理不同的数据。如果 Warp 内的线程执行了不同的指令路径（例如由于 if-else 分支），会导致 Warp 分化 (divergence)，降低执行效率。

线程的全局索引通常这样计算（以一维为例）：`int tid = blockIdx.x * blockDim.x + threadIdx.x;`

### CUDA 内存层次

GPU 拥有多种不同特性和访问速度的内存类型：

- **Registers (寄存器)：**每个线程私有的最快内存，数量有限。编译器会自动将频繁使用的变量分配到寄存器。
- **Shared Memory (共享内存)：**每个线程块内的所有线程共享的片上内存。访问速度远快于全局内存，是实现高性能 Kernel 的关键。需要程序员显式管理。
- **Global Memory (全局内存)：**所有线程都可以访问的设备主内存（通常是 DRAM）。容量最大，但访问延迟最高，带宽相对较低。
- **Constant Memory (常量内存)：**对于所有线程只读的内存区域，当 Warp 内所有线程访问相同地址时有缓存优化。
- **Texture Memory (纹理内存)：**针对图形应用优化，具有特定的缓存机制，适用于具有空间局部性的访问模式。

**Memory Coalescing (内存合并访问)：**当一个 Warp 中的线程访问全局内存时，如果它们访问的是连续的、对齐的内存地址，GPU可以将这些访问合并成少数几次内存事务，从而有效利用内存带宽。不合并的访问会导致多次内存事务，显著降低性能。使用共享内存可以帮助将非合并的全局内存访问模式转化为对共享内存的合并访问。

### 同步机制

- **`__syncthreads()`：**块内屏障同步，确保块内所有线程都到达同步点后才能继续执行。
- **Atomic Operations (原子操作)：**用于在多个线程并发读写同一内存位置时避免数据竞争。例如 `atomicAdd()`, `atomicExch()` 等。原子操作可以直接作用于全局内存或共享内存。

## 并行算法设计要点

设计高效的并行算法并没有简单的秘诀，它需要创造性的思维和经验积累。以下是一些关键的考虑因素 (lecture13_2024, p48)：

- **细粒度并行 (Fine-grained parallelism)：**将问题分解为大量可以独立执行的小任务，以充分利用 GPU 的大规模并行处理单元。
- **内存合并 (Memory coalescing)：**优化全局内存访问模式，确保 Warp 内线程访问连续内存，以最大化内存带宽利用率。
- **高效使用共享内存：**通过将数据缓存到共享内存来减少对全局内存的访问，提高数据复用率。
- **正确使用同步：**通过 `__syncthreads()` 和原子操作来协调线程间的数据共享和依赖关系，避免竞争条件和死锁。

## 结语

通过理解基础矩阵乘法的性能瓶颈，并采用如分块矩阵乘法这样的优化技术，可以显著提升 CUDA 程序在 GPU 上的执行效率。掌握共享内存、线程同步、边界处理以及 CUDA 编程模型的核心概念，是编写高性能并行计算应用的基础。不断实践、分析和调试是提升并行编程能力的关键。

## 附录：专业词汇解释

**CUDA (Compute Unified Device Architecture)**NVIDIA 推出的并行计算平台和编程模型，允许软件开发者和软件工程师使用 NVIDIA 图形处理单元 (GPU) 进行通用目的处理（即不仅仅是图形渲染）。**Kernel (内核函数)**在 CUDA 中，指在 GPU 设备上由大量线程并行执行的函数，使用 `__global__` 关键字声明。

**GPU (Graphics Processing Unit，图形处理器)**一种专门设计用来处理复杂图形和并行计算任务的微处理器。其拥有大量计算核心，非常适合执行大规模并行任务。

**GFLOPS (Giga Floating-point Operations Per Second)**每秒十亿次浮点运算次数，是衡量计算设备（特别是 GPU 和 CPU）浮点运算性能的单位。

**DRAM (Dynamic Random-Access Memory，动态随机存取存储器)**一种常见的计算机内存类型，GPU 的全局内存通常是基于 DRAM 技术的。

**Global Memory (全局内存)**GPU 上容量最大但访问速度相对较慢的内存空间，所有线程网格中的线程都可以访问。通常是 DDR SDRAM。

**Shared Memory (共享内存)**位于每个 SM (Streaming Multiprocessor) 上的片上高速缓存，由同一线程块内的所有线程共享。访问速度远快于全局内存，但容量有限。程序员可以显式控制其使用。**Thread (线程)**CUDA 中执行 Kernel 代码的最小单元。成千上万的线程可以并行执行同一个 Kernel 函数。

**Block (线程块)**一组线程的集合，组织成一维、二维或三维。同一块内的线程可以合作，通过共享内存交换数据，并通过 `__syncthreads()`进行同步。一个线程块会被调度到单个 SM 上执行。

**Grid (线程网格)**一次 Kernel 调用启动的所有线程块的集合，可组织成一维、二维或三维。一个 Grid 对应一次 Kernel 执行。

**Warp (线程束)**SM 上线程调度的基本单位，通常由 32 个线程组成。Warp 内的线程以 SIMD (单指令多数据) 方式执行相同的指令。**`__global__`**CUDA C/C++ 中的函数类型限定符，表示该函数是一个 Kernel，将从 CPU 调用并在 GPU 上执行。**`__shared__`**CUDA C/C++ 中的变量类型限定符，表示该变量存储在共享内存中，对同一线程块内的所有线程可见且共享。**`blockIdx`, `blockDim`, `threadIdx`**CUDA 内置变量。`threadIdx` 是线程在块内的索引；`blockDim` 是块的维度（线程数量）；`blockIdx` 是块在网格内的索引；`gridDim` (未在列表中但相关) 是网格的维度（块数量）。这些变量帮助线程确定其在整个计算任务中的唯一位置和负责的数据。**`__syncthreads()`**一个 CUDA 内置函数，用于在线程块内部设置一个同步点。块内的所有线程必须到达此同步点后，才能继续执行后续指令。确保数据依赖的正确性。

**SIMD (Single Instruction, Multiple Data，单指令多数据流)**一种并行计算的执行模型，其中多个处理单元使用相同的指令同时处理不同的数据。GPU 的 Warp 执行遵循此模型。

**Memory Coalescing (内存合并访问)**GPU 内存访问优化技术。当一个 Warp 中的线程访问全局内存时，如果它们的访问地址是连续且对齐的，GPU 可以将这些多次独立的访问请求合并成较少的大块内存事务，从而提高内存带宽利用率和访问效率。

**Atomic Operations (原子操作)**指那些在执行过程中不会被其他线程中断的操作。在并行编程中，原子操作用于安全地更新共享数据，避免由多个线程同时读写同一内存位置而引发的竞争条件。

**Fine-grained parallelism (细粒度并行)**一种并行计算策略，将任务分解成大量非常小的、可以独立或近乎独立执行的子任务。GPU 的架构非常适合利用细粒度并行。 

**SM (Streaming Multiprocessor，流式多处理器)**NVIDIA GPU 架构中的核心处理单元。每个 SM 包含一定数量的 CUDA核心 (用于运算)、共享内存、寄存器文件等资源，并负责调度和执行线程块中的 Warp。**TILE_WIDTH**在分块矩阵乘法中，用于定义子块 (Tile) 边长的参数。例如，一个 TILE_WIDTH x TILE_WIDTH 的子块。