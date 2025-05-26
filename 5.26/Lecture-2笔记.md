# Lecture-2笔记

# **悉尼大学计算机科学课程：并行编程实践笔记**

*基于 Bing Bing Zhou 教授的 HUST-USYD 暑期学校并行编程实践讲座 (Lecture 2, 2024) 整理*

---

## **目录导航**

- [引言：开启高效并行编程之旅](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%BC%95%E8%A8%80%E5%BC%80%E5%90%AF%E9%AB%98%E6%95%88%E5%B9%B6%E8%A1%8C%E7%BC%96%E7%A8%8B%E4%B9%8B%E6%97%85)
    - [并行计算的魅力与挑战](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%B9%B6%E8%A1%8C%E8%AE%A1%E7%AE%97%E7%9A%84%E9%AD%85%E5%8A%9B%E4%B8%8E%E6%8C%91%E6%88%98)
    - [性能优化的黄金法则](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96%E7%9A%84%E9%BB%84%E9%87%91%E6%B3%95%E5%88%99)
    - [本笔记导览](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E6%9C%AC%E7%AC%94%E8%AE%B0%E5%AF%BC%E8%A7%88)
- [核心基石：理解计算机的记忆与计算](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E6%A0%B8%E5%BF%83%E5%9F%BA%E7%9F%B3%E7%90%86%E8%A7%A3%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%9A%84%E8%AE%B0%E5%BF%86%E4%B8%8E%E8%AE%A1%E7%AE%97)
    - [内存层次结构 (Memory Hierarchy)](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%86%85%E5%AD%98%E5%B1%82%E6%AC%A1%E7%BB%93%E6%9E%84-memory-hierarchy)
    - [计算强度 (Computational Intensity)](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E8%AE%A1%E7%AE%97%E5%BC%BA%E5%BA%A6-computational-intensity)
- [性能调优利器：矩阵乘法优化](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E6%80%A7%E8%83%BD%E8%B0%83%E4%BC%98%E5%88%A9%E5%99%A8%E7%9F%A9%E9%98%B5%E4%B9%98%E6%B3%95%E4%BC%98%E5%8C%96)
    - [矩阵乘法：一个经典案例](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E7%9F%A9%E9%98%B5%E4%B9%98%E6%B3%95%E4%B8%80%E4%B8%AA%E7%BB%8F%E5%85%B8%E6%A1%88%E4%BE%8B)
    - [循环顺序优化](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%BE%AA%E7%8E%AF%E9%A1%BA%E5%BA%8F%E4%BC%98%E5%8C%96)
    - [分块技术 (Blocking)](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%88%86%E5%9D%97%E6%8A%80%E6%9C%AF-blocking)
    - [循环展开 (Loop Unrolling)](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%BE%AA%E7%8E%AF%E5%B1%95%E5%BC%80-loop-unrolling)
- [实践应用：从理论到代码](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%AE%9E%E8%B7%B5%E5%BA%94%E7%94%A8%E4%BB%8E%E7%90%86%E8%AE%BA%E5%88%B0%E4%BB%A3%E7%A0%81)
    - [矩阵乘法的实现](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E7%9F%A9%E9%98%B5%E4%B9%98%E6%B3%95%E7%9A%84%E5%AE%9E%E7%8E%B0)
    - [矩阵-向量乘法的优化](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E7%9F%A9%E9%98%B5-%E5%90%91%E9%87%8F%E4%B9%98%E6%B3%95%E7%9A%84%E4%BC%98%E5%8C%96)
    - [性能对比与分析](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E6%80%A7%E8%83%BD%E5%AF%B9%E6%AF%94%E4%B8%8E%E5%88%86%E6%9E%90)
- [总结与拓展](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E6%80%BB%E7%BB%93%E4%B8%8E%E6%8B%93%E5%B1%95)
    - [关键知识点回顾](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%85%B3%E9%94%AE%E7%9F%A5%E8%AF%86%E7%82%B9%E5%9B%9E%E9%A1%BE)
    - [进阶学习路径](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E8%BF%9B%E9%98%B6%E5%AD%A6%E4%B9%A0%E8%B7%AF%E5%BE%84)
    - [参考资源](https://www.tiangong.cn/project/1926958232849481728?net_search=true&from=home&query=#%E5%8F%82%E8%80%83%E8%B5%84%E6%BA%90)

---

## **引言：开启高效并行编程之旅**

### **并行计算的魅力与挑战**

在当今计算机科学领域，并行编程已成为提升程序性能的关键技术。随着硬件架构的发展，单纯依靠提高处理器频率来提升性能的时代已经结束，取而代之的是多核心、多线程的并行计算模式。这种转变带来了性能提升的机遇，同时也增加了编程的复杂性。

对于并行编程的初学者来说，了解如何有效利用硬件资源、避免性能瓶颈至关重要。本笔记将从内存层次结构、计算强度等基础概念入手，通过矩阵乘法这一经典案例，为您揭示并行编程优化的核心原理和实用技巧。

### **性能优化的黄金法则**

在深入技术细节之前，我们先了解几条并行编程优化的黄金法则：

1. **数据局部性优先**：尽可能充分利用缓存，减少内存访问延迟
2. **计算与访存平衡**：提高计算强度，减少内存访问次数
3. **避免依赖与冲突**：减少线程间的数据依赖和资源冲突
4. **分而治之**：将大问题分解为可并行处理的小问题

这些原则将贯穿本笔记的始终，引导我们理解并行编程优化的核心思想。

### **本笔记导览**

本笔记将按照从基础到应用的顺序，系统介绍并行编程优化的关键概念和技术：

- 首先，我们将学习**内存层次结构**和**计算强度**这两个核心概念
- 然后，通过**矩阵乘法**这一经典案例，深入了解循环顺序、分块技术、循环展开等优化方法
- 最后，我们将学习如何将这些理论应用到实际代码中，并分析不同优化策略的性能差异

每个概念都会配以详细解释和图表示例，专业术语会通过脚注或内联方式进行解释，确保初学者也能轻松理解。

---

## **核心基石：理解计算机的记忆与计算**

### **内存层次结构 (Memory Hierarchy)**

内存层次结构是理解计算机性能的关键概念。简单来说，它是指计算机中不同类型存储器按照速度、容量和成本组织而成的层级体系。

### **什么是内存层次结构？**

内存层次结构类似于一个金字塔，从上到下依次是：

1. **寄存器 (Registers)**：速度最快，容量最小，直接位于CPU内部
2. **缓存 (Cache)**：分为多级 (L1/L2/L3)，速度较快，容量适中
3. **主内存 (Main Memory/RAM)**：速度中等，容量较大
4. **辅助存储 (Secondary Storage)**：如硬盘，速度慢，容量大

### **为什么内存层次结构很重要？**

内存层次结构之所以重要，是因为不同层级的访问速度差异巨大：

- 寄存器访问：~1个时钟周期（<1ns）
- L1缓存访问：~3-4个时钟周期（~1ns）
- L2缓存访问：~10-20个时钟周期（~3-7ns）
- L3缓存访问：~40-75个时钟周期（~15-25ns）
- 主内存访问：~100-300个时钟周期（~60-100ns）

这意味着，从主内存读取数据可能比从L1缓存读取数据慢数十倍！因此，高效利用缓存成为程序性能优化的关键。

### **如何利用内存层次结构？**

优化程序以充分利用内存层次结构的核心策略是提高**数据局部性 (Data Locality)**，包括：

1. **时间局部性 (Temporal Locality)**：最近访问过的数据很可能在不久后再次被访问
    - 实现方法：循环中重复使用的变量尽量保持在寄存器中
2. **空间局部性 (Spatial Locality)**：访问某个内存位置附近的数据的概率较高
    - 实现方法：连续访问内存中相邻的元素，如按行访问二维数组
3. **分块访问 (Blocking)**：将大数据集分解为适合缓存大小的小块
    - 实现方法：矩阵乘法中的分块计算（后面会详细讲解）

### **计算强度 (Computational Intensity)**

计算强度是衡量算法效率的重要指标，它描述了程序中计算操作与内存访问操作的比例。

### **什么是计算强度？**

计算强度的定义为：

```

计算强度 = 算术操作次数 / 内存访问次数
```

- **算术操作**：加、减、乘、除等计算指令
- **内存访问**：从内存读取数据或写入数据的操作

计算强度越高，表示程序在每次从内存获取数据后，执行了更多的计算操作，这通常意味着更高的效率。

### **为什么计算强度很重要？**

计算强度之所以重要，是因为现代计算机中，内存访问通常是主要的性能瓶颈：

- CPU的计算速度远快于内存的访问速度
- 内存带宽有限，而且访问延迟高
- 高计算强度的算法可以更好地隐藏内存延迟

当我们提高算法的计算强度时，能够减少内存访问的比例，从而提升整体性能。

### **如何提高计算强度？**

提高计算强度的常用策略包括：

1. **数据重用**：尽可能多次使用已经加载到缓存或寄存器中的数据
2. **循环合并**：将多个循环合并，减少中间结果的存储和加载
3. **循环展开**：减少循环控制开销，提高指令级并行度
4. **向量化计算**：利用SIMD指令一次处理多个数据元素

在后续的矩阵乘法优化中，我们将看到这些策略的具体应用。

---

## **性能调优利器：矩阵乘法优化**

### **矩阵乘法：一个经典案例**

矩阵乘法是并行编程优化的经典案例，它具有规则的计算模式和丰富的优化空间，是学习性能调优的理想例子。

### **基本定义**

给定两个矩阵 A(m×k) 和 B(k×n)，它们的乘积 C = A × B 是一个 m×n 的矩阵，其中:

$$
⁍
$$

### **朴素实现**

最直接的矩阵乘法实现（三重循环）如下：

```c
c
for (i = 0; i < M; i++)
    for (j = 0; j < N; j++) {
        C[i][j] = 0;
        for (p = 0; p < K; p++)
            C[i][j] += A[i][p] * B[p][j];
    }
```

这种实现简单明了，但性能较差，主要原因是没有充分利用缓存。特别是对矩阵B的访问是按列进行的，违反了内存的连续访问模式（在C语言中，二维数组按行存储）。

### **循环顺序优化**

循环顺序对性能的影响巨大，因为它决定了内存访问模式。

### **六种循环顺序**

矩阵乘法的三重循环（i、j、p）可以有6种不同的排列顺序：ijk, ikj, jik, jki, kij, kji。不同的顺序会导致不同的内存访问模式，从而影响缓存利用率。

我们来分析几种常见的循环顺序：

**1. ijk顺序（朴素实现）**

```c
c
for (i = 0; i < M; i++)
    for (j = 0; j < N; j++)
        for (p = 0; p < K; p++)
            C[i][j] += A[i][p] * B[p][j];
```

- A按行访问（良好的空间局部性）
- B按列访问（糟糕的空间局部性）
- C的每个元素被反复更新（良好的时间局部性）

**2. ikj顺序**

```c
c
for (i = 0; i < M; i++)
    for (p = 0; p < K; p++)
        for (j = 0; j < N; j++)
            C[i][j] += A[i][p] * B[p][j];
```

- A按行访问（良好的空间局部性）
- B按行访问（良好的空间局部性）
- C按行访问（良好的空间局部性）

这种顺序的性能通常优于ijk顺序，因为对矩阵B的访问是连续的。

**3. kij顺序**

```c
c
for (p = 0; p < K; p++)
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
            C[i][j] += A[i][p] * B[p][j];
```

- A的每一行被重复扫描（不理想的时间局部性）
- B按行访问（良好的空间局部性）
- C按行访问（良好的空间局部性）

### **性能对比**

不同循环顺序的性能对比如下（以ijk顺序为基准）：

| **循环顺序** | **相对性能** | **空间局部性** |
| --- | --- | --- |
| ijk | 1.0x | 中 |
| ikj | ~1.5-2x | 高 |
| jik | ~0.9x | 中 |
| jki | ~0.9x | 中 |
| kij | ~1.3x | 高 |
| kji | ~0.8x | 低 |

实际上，ikj顺序通常是这六种顺序中性能最好的，因为它对所有三个矩阵都具有良好的访问模式。

### **分块技术 (Blocking)**

分块是一种将大型数据集分解为适合缓存大小的小块的技术，能够显著提高缓存命中率。

### **什么是分块？**

分块技术的核心思想是：不一次性处理整个矩阵，而是将矩阵分割成多个小块，并在这些小块上执行计算。这样可以确保一个小块的数据在完成所有相关计算前都保持在缓存中。

### **分块实现**

以下是矩阵乘法的分块实现：

```c
c
// B是块大小（通常选择适合缓存大小的值，如32或64）for (i = 0; i < M; i += B)
    for (j = 0; j < N; j += B)
        for (p = 0; p < K; p += B)
// 处理块内元素for (ii = i; ii < min(i+B, M); ii++)
                for (jj = j; jj < min(j+B, N); jj++)
                    for (pp = p; pp < min(p+B, K); pp++)
                        C[ii][jj] += A[ii][pp] * B[pp][jj];
```

### **分块的优势**

分块技术带来的主要优势包括：

1. **提高缓存命中率**：确保数据块适合缓存大小，减少缓存未命中
2. **减少内存带宽压力**：减少对主内存的访问次数
3. **提高数据重用**：块内数据可以被多次使用而无需重新从内存加载

选择合适的块大小是关键，它应当基于目标系统的缓存大小。理想情况下，三个矩阵（A、B、C）的工作集应当适合缓存大小。

### **循环展开 (Loop Unrolling)**

循环展开是一种减少循环控制开销并提高指令级并行度的技术。

### **什么是循环展开？**

循环展开是指将循环体内的代码复制多次，每次迭代处理多个元素，从而减少循环迭代次数的技术。

例如，将这样的循环：

```c
c
for (i = 0; i < N; i++)
    a[i] = b[i] + c[i];
```

展开为：

```c
c
for (i = 0; i < N; i += 4) {
    a[i] = b[i] + c[i];
    a[i+1] = b[i+1] + c[i+1];
    a[i+2] = b[i+2] + c[i+2];
    a[i+3] = b[i+3] + c[i+3];
}
```

### **循环展开的优势**

循环展开带来的主要优势包括：

1. **减少循环控制开销**：减少循环计数、条件检查和分支预测的次数
2. **提高指令级并行度**：允许编译器生成更多的并行指令
3. **更好地利用寄存器**：一次处理多个数据项，提高寄存器利用率
4. **减少分支预测失败**：减少循环跳转的次数，提高分支预测准确率

### **应用于矩阵乘法**

将循环展开应用于矩阵乘法，以内层循环（j）为例：

```c
c
for (i = 0; i < M; i++)
    for (j = 0; j < N; j += 4) {
// 确保N是4的倍数，或处理边界情况
        C[i][j] += A[i][p] * B[p][j];
        C[i][j+1] += A[i][p] * B[p][j+1];
        C[i][j+2] += A[i][p] * B[p][j+2];
        C[i][j+3] += A[i][p] * B[p][j+3];
    }
```

循环展开的最佳展开因子取决于硬件平台、编译器优化能力和具体问题，通常为2、4或8。

### **循环展开的注意事项**

虽然循环展开能提高性能，但也有一些需要注意的地方：

1. **代码膨胀**：展开后的代码体积更大，可能影响指令缓存
2. **寄存器压力**：过度展开可能导致寄存器不足，引发寄存器溢出
3. **边界条件**：需要处理迭代次数不是展开因子倍数的情况
4. **维护成本**：展开的代码可读性较差，维护难度增加

---

## **实践应用：从理论到代码**

### **矩阵乘法的实现**

下面我们将前面学习的优化技术应用到矩阵乘法的实际实现中。

### **基准实现（未优化）**

```c
c
void matrix_multiply_baseline(double **A, double **B, double **C, int M, int N, int K) {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0;
            for (int p = 0; p < K; p++)
                C[i][j] += A[i][p] * B[p][j];
        }
}
```

### **优化循环顺序**

```c
c
void matrix_multiply_optimized_loop(double **A, double **B, double **C, int M, int N, int K) {
// 初始化结果矩阵for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = 0.0;

// ikj顺序（通常性能最佳）for (int i = 0; i < M; i++)
        for (int p = 0; p < K; p++)
            for (int j = 0; j < N; j++)
                C[i][j] += A[i][p] * B[p][j];
}
```

### **分块优化**

```c
c
void matrix_multiply_blocked(double **A, double **B, double **C, int M, int N, int K) {
// 初始化结果矩阵for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = 0.0;

// 定义块大小（应根据缓存大小调整）const int BLOCK_SIZE = 32;

// 分块矩阵乘法for (int i = 0; i < M; i += BLOCK_SIZE)
        for (int j = 0; j < N; j += BLOCK_SIZE)
            for (int p = 0; p < K; p += BLOCK_SIZE)
// 处理块内元素for (int ii = i; ii < min(i+BLOCK_SIZE, M); ii++)
                    for (int pp = p; pp < min(p+BLOCK_SIZE, K); pp++) {
                        double a_ip = A[ii][pp];// 缓存A中的元素for (int jj = j; jj < min(j+BLOCK_SIZE, N); jj++)
                            C[ii][jj] += a_ip * B[pp][jj];
                    }
}
```

### **循环展开与分块结合**

```c
c
void matrix_multiply_blocked_unrolled(double **A, double **B, double **C, int M, int N, int K) {
// 初始化结果矩阵for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = 0.0;

// 定义块大小（应根据缓存大小调整）const int BLOCK_SIZE = 32;

// 分块矩阵乘法for (int i = 0; i < M; i += BLOCK_SIZE)
        for (int j = 0; j < N; j += BLOCK_SIZE)
            for (int p = 0; p < K; p += BLOCK_SIZE)
// 处理块内元素for (int ii = i; ii < min(i+BLOCK_SIZE, M); ii++)
                    for (int pp = p; pp < min(p+BLOCK_SIZE, K); pp++) {
                        double a_ip = A[ii][pp];// 缓存A中的元素for (int jj = j; jj < min(j+BLOCK_SIZE, N); jj += 4) {
// 处理边界情况if (jj + 3 < min(j+BLOCK_SIZE, N)) {
// 循环展开4次
                                C[ii][jj] += a_ip * B[pp][jj];
                                C[ii][jj+1] += a_ip * B[pp][jj+1];
                                C[ii][jj+2] += a_ip * B[pp][jj+2];
                                C[ii][jj+3] += a_ip * B[pp][jj+3];
                            } else {
// 处理边界情况for (int j_end = jj; j_end < min(j+BLOCK_SIZE, N); j_end++)
                                    C[ii][j_end] += a_ip * B[pp][j_end];
                            }
                        }
                    }
}
```

### **矩阵-向量乘法的优化**

除了矩阵-矩阵乘法，矩阵-向量乘法也是一个常见的计算核心。它的优化策略与矩阵乘法类似，但有一些针对性的调整。

### **基本定义**

给定矩阵A(m×n)和向量x(n)，矩阵-向量乘法计算结果向量y(m)：

$$
y[i] = ∑(A[i,j] × x[j]) for j = 0 to n-1
$$

### **基准实现**

```c
c
void matrix_vector_multiply_baseline(double **A, double *x, double *y, int M, int N) {
    for (int i = 0; i < M; i++) {
        y[i] = 0.0;
        for (int j = 0; j < N; j++)
            y[i] += A[i][j] * x[j];
    }
}
```

### **循环展开优化**

```c
c
void matrix_vector_multiply_unrolled(double **A, double *x, double *y, int M, int N) {
    for (int i = 0; i < M; i++) {
        y[i] = 0.0;
        int j = 0;
// 循环展开4次for (; j + 3 < N; j += 4) {
            y[i] += A[i][j] * x[j] +
                   A[i][j+1] * x[j+1] +
                   A[i][j+2] * x[j+2] +
                   A[i][j+3] * x[j+3];
        }
// 处理剩余元素for (; j < N; j++)
            y[i] += A[i][j] * x[j];
    }
}
```

### **缓存优化（转置技术）**

对于大型矩阵，可以考虑转置矩阵A来提高缓存命中率：

```c
c
void matrix_vector_multiply_transpose(double **A, double *x, double *y, int M, int N) {
// 初始化结果向量for (int i = 0; i < M; i++)
        y[i] = 0.0;

// 转置计算方法（按列计算）for (int j = 0; j < N; j++) {
        double x_j = x[j];// 缓存x中的元素for (int i = 0; i < M; i++)
            y[i] += A[i][j] * x_j;
    }
}
```

### **性能对比与分析**

不同优化策略对性能的影响可能会因硬件平台、矩阵大小等因素而异。以下是一个典型的性能对比：

### **矩阵乘法优化性能对比**

假设在一个主流处理器上，对1024×1024矩阵进行乘法运算：

| **优化策略** | **相对性能** | **优化收益** |
| --- | --- | --- |
| 基准实现（未优化） | 1.0x | - |
| 循环顺序优化 (ikj) | ~1.8x | +80% |
| 仅分块优化 | ~2.5x | +150% |
| 分块+循环展开 | ~3.2x | +220% |
| 手动SIMD优化 | ~4.5x | +350% |
| 多线程并行 | ~8-16x（视核心数） | +700-1500% |

### **不同优化策略的适用场景**

| **优化策略** | **最适合的场景** | **实现难度** |
| --- | --- | --- |
| 循环顺序优化 | 任何矩阵乘法，特别是中小型矩阵 | 低 |
| 分块优化 | 大型矩阵，特别是超过缓存大小的矩阵 | 中 |
| 循环展开 | 规则形状的矩阵，有良好对齐的数据 | 中 |
| SIMD优化 | 数据类型固定，结构规则的计算 | 高 |
| 多线程并行 | 计算密集型任务，独立数据块处理 | 高 |

### **性能调优的一般步骤**

1. **基准测试**：测量初始实现的性能，建立基准
2. **分析瓶颈**：使用性能分析工具找出热点和瓶颈
3. **应用基本优化**：循环顺序、数据局部性优化等
4. **高级优化**：分块、循环展开、SIMD指令等
5. **并行化**：多线程、向量化等
6. **测量与验证**：确保优化正确性并测量性能提升

---

## **总结与拓展**

### **关键知识点回顾**

通过本笔记，学习了：

1. **内存层次结构**：理解寄存器、缓存和主内存的关系，以及数据局部性的重要性
2. **计算强度**：优化计算操作与内存访问的比率，提高程序效率
3. **矩阵乘法优化**：
    - 循环顺序优化：选择最适合缓存模式的循环顺序
    - 分块技术：将大矩阵分解为适合缓存的小块
    - 循环展开：减少循环控制开销，提高指令级并行度
4. **实践应用**：将理论应用到矩阵乘法和矩阵-向量乘法的实际实现中

这些知识和技术不仅适用于矩阵运算，还可以应用于其他计算密集型任务的优化。

### **进阶学习路径**

1. **SIMD编程**：学习使用SSE/AVX等指令集进行向量化计算
2. **多线程并行编程**：OpenMP, Pthreads等技术
3. **GPU编程**：CUDA, OpenCL等面向图形处理器的并行编程
4. **分布式计算**：MPI等分布式内存编程模型
5. **自动调优和性能建模**：使用自动调优技术优化并行程序

---

**专业术语对照表**：

- Memory Hierarchy (内存层次结构)
- Computational Intensity (计算强度)
- Spatial Locality (空间局部性)
- Temporal Locality (时间局部性)
- Blocking/Tiling (分块技术)
- Loop Unrolling (循环展开)
- Cache (缓存)
- Register (寄存器)
- Instruction-Level Parallelism (指令级并行)
- SIMD (单指令多数据)
- Cache Miss (缓存未命中)
- Memory Bandwidth (内存带宽)
- Cache Line (缓存线)
- Prefetching (预取)
- Branch Prediction (分支预测)