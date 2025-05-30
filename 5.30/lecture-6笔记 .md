# lecture-6笔记

# 悉尼大学平行编程实践夏季学校第六讲：任务分配与负载均衡概述

## 1. 并行计算性能与加速比

并行计算的目标是通过多个处理器同时工作来缩短程序的执行时间。衡量并行性能的一个关键指标是**加速比 (Speedup)**。加速比的定义是：

- **加速比** = 串行执行时间 / 并行执行时间。理想情况下，如果有 *p* 个处理器，加速比可以达到 *p* 倍（即每个处理器承担等量工作且无额外开销）。然而，实际中由于各种开销，加速比往往小于理想值，且随着处理器数量增加，实际加速比的提升会逐渐趋缓。

### 1.1 影响并行性能的因素

导致并行效率不高的主要原因包括：

- **单处理器性能不佳**：例如内存系统瓶颈，导致单个线程的执行速度受限。
- **并行开销过大**：包括线程/进程的**同步 (Synchronization)** 开销、**通信 (Communication)** 开销（不同处理器之间传递数据的时间）以及**冗余计算**等。
- **负载不均衡 (Load Imbalance)**：不同处理器分配的任务量差异较大，部分处理器提前完成任务后空闲等待，而其他处理器仍在忙碌。
- **计算与通信的比率不合理**：如果通信时间占总时间比例过高，并行效率会下降。
- **异构计算资源**：例如同时使用CPU和GPU时，不同设备的计算速度差异可能导致性能瓶颈。

### 1.2 阿姆达尔定律 (Amdahl's Law)

阿姆达尔定律描述了程序中串行部分对可能获得的加速比的限制：

```
加速比 ≤ 1 / (s + (1-s)/p)

```

其中：

- s 是程序中串行执行部分的比例
- p 是处理器数量
- (1-s) 是可并行化的部分比例

这表明即使很小的串行部分也会显著限制最大可能的加速比。同理，即使很小的负载不均衡也会显著限制最大速度提升。

## 2. 任务分配策略

任务分配是将一个大任务分解为若干子任务，并将这些子任务分配给不同的处理器执行的过程。良好的任务分配策略可以确保各处理器的负载均衡，并尽量减少处理器之间的通信。根据分配时机的不同，任务分配策略主要分为**静态分配**和**动态分配**两大类：

### 2.1 静态任务分配 (Static Task Assignment)

在程序运行前（编译时或启动时）就确定每个处理器的任务分配，运行时不再调整。这种策略简单且运行时开销为零。

**适用场景**：

- 各子任务的工作量可以预先估计且变化不大
- 稠密矩阵运算
- 规则网格问题
- 数据并行场景，其中每个数据项的处理时间大致相同

**示例代码（基于OpenMP的静态循环分配）**：

```c
#include <stdio.h>
#include <omp.h>

#define N 1000000

int main() {
    double sum = 0.0;

    // 静态分配，每个线程处理连续的数据块
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (int i = 0; i < N; i++) {
        // 假设每次迭代的计算量相同
        double value = i * 0.5;
        sum += value;
    }

    printf("Sum = %f\n", sum);
    return 0;
}

```

在这个例子中，`schedule(static)`意味着循环迭代将被**静态且均匀**地分配给各线程。例如，如果有4个线程，那么线程0处理迭代0-249999，线程1处理250000-499999，以此类推。

**优势**：

- 实现简单
- 无调度开销
- 任务分配可以在编译时或启动时完成

**劣势**：

- 不适合工作量不均衡或难以预测的场景
- 不能根据运行时条件进行调整

### 2.2 动态任务分配 (Dynamic Task Assignment)

在程序运行过程中根据实际情况动态地重新分配任务。当各子任务的执行时间难以预先估计，或者任务执行时间差异较大时，动态分配可以更好地平衡负载。

**适用场景**：

- 任务执行时间差异大或难以预测
- 异构计算环境（不同处理器性能不同）
- 递归算法或任务生成模式不规则的算法
- 负载可能动态变化的应用

**示例代码（基于OpenMP的动态循环分配）**：

```c
#include <stdio.h>
#include <omp.h>
#include <math.h>

#define N 1000000

// 模拟不同迭代计算量不同的场景
double compute(int i) {
    // 假设计算量随i变化，某些值需要更多计算
    if (i % 100 == 0)
        for (int j = 0; j < 10000; j++) { /* 额外计算 */ }
    return sin(i) * cos(i);
}

int main() {
    double sum = 0.0;

    // 动态分配，使用任务队列动态分配迭代
    #pragma omp parallel for reduction(+:sum) schedule(dynamic, 100)
    for (int i = 0; i < N; i++) {
        double value = compute(i);
        sum += value;
    }

    printf("Sum = %f\n", sum);
    return 0;
}

```

在这个例子中，`schedule(dynamic, 100)`表示每次从任务队列中取出100个迭代分配给请求工作的线程。当线程完成其分配的迭代后，会再次从队列请求更多工作，直到没有剩余迭代。这样，如果某些迭代需要更长时间，其他线程可以完成更多的"简单"迭代，从而实现动态负载均衡。

**优势**：

- 适应工作量不均衡的情况
- 能够根据运行时条件调整
- 在异构环境中性能更好

**劣势**：

- 有调度开销（任务队列管理、线程同步等）
- 可能导致缓存局部性不佳
- 实现更复杂

## 3. 负载均衡的重要性

**负载均衡 (Load Balancing)** 是指将工作量合理分配给各个处理单元，使得所有处理单元都能高效工作，尽量避免某些处理单元忙碌而其他处理单元空闲的情况。

根据阿姆达尔定律，即使很小的负载不均衡也会显著限制最大加速比。例如，如果在8个处理器上，其中一个处理器的负载比其他处理器多10%，那么最大加速比将不超过7.41，而不是理想的8。

```
假设理想情况下的执行时间为T，则：
- 7个处理器执行时间为T
- 1个处理器执行时间为1.1T
- 整体执行时间由最慢的处理器决定，即1.1T
- 加速比 = 串行时间(8T) / 并行时间(1.1T) = 7.27

```

这表明负载不均衡的影响很大，因此在设计并行算法时必须特别关注负载均衡问题。

## 4. 图划分与负载均衡

对于许多复杂的并行问题（如非结构化网格上的有限元计算、大规模图算法等），可以将其建模为图（Graph），然后通过**图划分 (Graph Partitioning)** 方法将图分解为若干子图，每个子图对应一个处理器的任务，从而实现负载均衡并减少处理器间的通信。

### 4.1 图划分的定义与目标

给定一个图G = (V, E)，图划分就是将顶点集V分割为k个不相交的子集V₁, V₂, ..., Vₖ，每个子集对应一个处理器的工作负载。好的图划分应同时满足：

1. **负载均衡**：每个子集中的顶点数量大致相同，或者每个子集的权重总和大致相同（如果顶点有不同的权重）。
2. **最小割集**：连接不��子集顶点的边数最少，这样可以减少处理器之间的通信量。

### 4.2 图划分算法

### 4.2.1 坐标二分法 (Coordinate Bisection)

当图的顶点具有几何坐标时（如有限元网格），可以基于几何坐标进行划分：

1. 选择一个维度（如x轴）
2. 沿该维度将顶点排序
3. 在中点处切分，得到两个子图
4. 递归地对子图继续划分，直到达到所需的分区数量

```python
def coordinate_bisection(vertices, depth=0, max_depth=3):
    if depth == max_depth:
        return [vertices]

    # 选择划分维度（交替使用x, y, z轴）
    dim = depth % 3  # 假设是3D空间

    # 按选定维度排序
    sorted_vertices = sorted(vertices, key=lambda v: v[dim])

    # 在中点处划分
    mid = len(sorted_vertices) // 2
    left_half = sorted_vertices[:mid]
    right_half = sorted_vertices[mid:]

    # 递归划分
    return (coordinate_bisection(left_half, depth+1, max_depth) +
            coordinate_bisection(right_half, depth+1, max_depth))

```

### 4.2.2 惯性二分法 (Inertial Bisection)

惯性二分法是坐标二分法的改进，考虑了顶点的分布特性：

1. 计算顶点集合的主方向（即具有最大惯性的方向）
2. 沿主方向投影顶点
3. 在投影的中点处切分
4. 递归划分子图

这种方法对于形状不规则的图效果更好，因为它会沿着图的"自然方向"进行切分。

### 4.2.3 谱二分法 (Spectral Bisection)

当图没有几何信息时，可以使用谱方法：

1. 计算图的拉普拉斯矩阵
2. 计算拉普拉斯矩阵的特征向量（尤其是对应第二小特征值的特征向量，又称Fiedler向量）
3. 根据Fiedler向量的值将顶点分为两组
4. 递归地对子图继续划分

```python
import numpy as np
from scipy import sparse
from scipy.sparse.linalg import eigsh

def spectral_bisection(adj_matrix):
    # 计算度矩阵
    n = adj_matrix.shape[0]
    degrees = np.sum(adj_matrix, axis=1)
    D = sparse.diags(degrees.flatten())

    # 计算拉普拉斯矩阵 L = D - A
    L = D - adj_matrix

    # 计算第二小特征值对应的特征向量
    eigenvalues, eigenvectors = eigsh(L, k=2, sigma=0, which='LM')
    fiedler_vector = eigenvectors[:, 1]  # 第二小特征值对应的特征向量

    # 根据Fiedler向量值的符号划分顶点
    partition = [[], []]
    for i in range(n):
        if fiedler_vector[i] < 0:
            partition[0].append(i)
        else:
            partition[1].append(i)

    return partition

```

### 4.2.4 多级方法 (Multilevel Methods)

对于大规模图，常用多级方法来改进效率：

1. **粗化 (Coarsening)**：通过合并顶点减小图的规模
2. **初始划分**：对粗化后的图进行划分
3. **细化 (Refinement)**：将划分结果映射回原图，并进行局部优化

多级方法是目前最有效的图划分算法之一，如METIS和Scotch等软件包都采用了这种方法。

### 4.3 图划分在实际应用中的使用

在实际应用中，图划分通常通过专业的库来实现，如：

- **METIS**：高效的图划分库，支持多种划分目标
- **ParMETIS**：METIS的并行版本
- **Scotch/PT-Scotch**：另一个流行的图划分库
- **Zoltan**：提供多种划分算法的综合库

**示例代码（使用PyMETIS进行图划分）**：

```python
import pymetis
import numpy as np

# 创建示例图（以邻接表形式）
# 每个顶点的邻居列表
adjacency = [
    [1, 2],      # 顶点0连接到顶点1和2
    [0, 2, 3],   # 顶点1连接到顶点0,2,3
    [0, 1, 3],   # 顶点2连接到顶点0,1,3
    [1, 2, 4, 5],# 顶点3连接到顶点1,2,4,5
    [3, 5],      # 顶点4连接到顶点3和5
    [3, 4]       # 顶点5连接到顶点3和4
]

# 转换为CSR格式
xadj = [0]
adjncy = []

for neighbors in adjacency:
    adjncy.extend(neighbors)
    xadj.append(xadj[-1] + len(neighbors))

# 为顶点分配权重（可选）
vweights = [1] * len(adjacency)

# 划分为2个子图
n_parts = 2
_, partitions = pymetis.part_graph(n_parts, xadj=xadj, adjncy=adjncy, vweights=vweights)

print(f"划分结果: {partitions}")
print(f"子图0的顶点: {[i for i, p in enumerate(partitions) if p == 0]}")
print(f"子图1的顶点: {[i for i, p in enumerate(partitions) if p == 1]}")

```

## 5. OpenMP中的循环调度

OpenMP是一个常用的共享内存并行编程接口，它提供了多种循环调度策略来处理负载均衡问题。在OpenMP中，可以通过`schedule`子句指定循环迭代的分配方式。

### 5.1 OpenMP提供的循环调度类型

1. **静态调度 (Static)**：迭代被静态地、均匀地分配给线程，默认情况下每个线程获得相同数量的连续迭代。

```c
#pragma omp parallel for schedule(static)
for (int i = 0; i < N; i++) {
    // 工作量均匀的任务
}

```

静态调度还可以指定块大小：

```c
#pragma omp parallel for schedule(static, 100)

```

这表示每个线程每次获取100个连续的迭代。

1. **动态调度 (Dynamic)**：迭代块动态分配给线程。一旦线程完成分配的块，就会请求更多迭代。

```c
#pragma omp parallel for schedule(dynamic, 50)
for (int i = 0; i < N; i++) {
    // 工作量不均匀的任务
}

```

这里的50是块大小，表示每次分配50个迭代给请求工作的线程。

1. **引导调度 (Guided)**：类似动态调度，但块大小随着剩余迭代减少而减小，从而减少调度开销。

```c
#pragma omp parallel for schedule(guided, 10)
for (int i = 0; i < N; i++) {
    // 复杂的任务，可能需要多次调整
}

```

初始块大小较大，随着剩余工作减少，块大小逐渐减小，直到达到指定的最小块大小（这里是10）。

1. **运行时调度 (Runtime)**：调度类型在运行时由环境变量`OMP_SCHEDULE`决定。

```c
#pragma omp parallel for schedule(runtime)
for (int i = 0; i < N; i++) {
    // 任务
}

```

这使得程序可以在不重新编译的情况下更改调度策略。

1. **自动调度 (Auto)**：调度由编译器和运行时系统决定。

```c
#pragma omp parallel for schedule(auto)
for (int i = 0; i < N; i++) {
    // 任务
}

```

### 5.2 选择合适的调度策略

选择哪种调度策略取决于应用的特性：

- **静态调度**：适合各迭代计算量相近的情况，调度开销最小，缓存局部性最好。
- **动态调度**：适合各迭代计算量差异较大或无法预测的情况，可以更好地平衡负载，但有调度开销。
- **引导调度**：在动态调度和开销控制之间取得平衡，适合有较大负载不均衡但又希望减少调度开销的情况。
- **运行时调度**：适合需要灵活调整调度策略的应用，无需重新编译。
- **自动调度**：当不确定哪种调度最佳时，可以尝试让编译器/运行时系统选择。

### 5.3 实例：矩阵乘法中的负载均衡

考虑稠密矩阵乘法 C = A × B，其中A、B和C都是n×n矩阵：

```c
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

void matrix_multiply(double A[N][N], double B[N][N], double C[N][N], const char* schedule_type) {
    double start_time = omp_get_wtime();

    if (strcmp(schedule_type, "static") == 0) {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                C[i][j] = 0.0;
                for (int k = 0; k < N; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    } else if (strcmp(schedule_type, "dynamic") == 0) {
        #pragma omp parallel for schedule(dynamic, 10)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                C[i][j] = 0.0;
                for (int k = 0; k < N; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    } else if (strcmp(schedule_type, "guided") == 0) {
        #pragma omp parallel for schedule(guided)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                C[i][j] = 0.0;
                for (int k = 0; k < N; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }

    double end_time = omp_get_wtime();
    printf("Schedule %s took: %f seconds\n", schedule_type, end_time - start_time);
}

int main() {
    // 分配和初始化矩阵...

    // 测试不同调度策略
    matrix_multiply(A, B, C, "static");
    matrix_multiply(A, B, C, "dynamic");
    matrix_multiply(A, B, C, "guided");

    // 释放内存...
    return 0;
}

```

在这个例子中，由于每行计算量相似，静态调度可能表现最好，因为它有最少的调度开销和最好的数据局部性。

## 6. 任务并行和工作窃取

除了数据并行（如循环并行化）外，任务并行是另一种常见的并行模式，特别适合递归算法或不规则问题。

### 6.1 OpenMP任务模型

从OpenMP 3.0开始引入了任务并行模型，允许动态创建任务：

```c
#include <stdio.h>
#include <omp.h>

void process_tree(node* p) {
    // 处理当前节点
    process_node(p);

    // 并行处理子节点
    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int i = 0; i < p->num_children; i++) {
                #pragma omp task
                process_tree(p->children[i]);
            }
        }
    }
}

```

在这个例子中，`#pragma omp task`指令创建新任务来处理树的每个子节点，这些任务可以被任何可用的线程执行。

### 6.2 工作窃取 (Work Stealing)

工作窃取是一种动态负载均衡技术，当一个线程完成其分配的所有任务后，可以"窃取"其他忙碌线程队列中的任务：

1. 每个线程维护一个本地任务队列
2. 当线程创建新任务时，将其放入本地队列
3. 当线程空闲时，它尝试从其他线程的队列末尾窃取任务
4. 线程优先处理自己队列前端的任务，这有利于缓存局部性

许多现代任务并行运行时系统（包括OpenMP、TBB、Cilk等）都采用工作窃取机制来实现动态负载均衡。

## 7. 结语与最佳实践

在并行编程中实现高效的任务分配和负载均衡需要考虑以下因素：

1. **了解问题特性**：
    - 任务是否规则？
    - 计算量是否可预测？
    - 数据依赖模式如何？
2. **选择适当的任务分配策略**：
    - 规则、可预测的问题 → 静态分配
    - 不规则、不可预测的问题 → 动态分配
    - 复杂的网格或图结构 → 专业图划分算法
3. **避免过细粒度并行**：
    - 任务太小会导致调度开销超过并行收益
    - 适当合并小任务或使用适当的块大小
4. **考虑数据局部性**：
    - 尽量让相关数据在同一处理器上处理，减少通信
    - 静态调度通常有更好的数据局部性
5. **使用适当的同步机制**：
    - 避免过度同步
    - 使用细粒度锁代替全局锁
    - 考虑无锁算法
6. **针对特定问题定制解决方案**：
    - 通用方法不一定适合所有场景
    - 可能需要混合使用多种策略

通过深入理解任务分配和负载均衡技术，我们可以设计出高效的并行算法，充分利用现代多核和分布式计算系统的潜力。

## 专业术语表

| 术语 | 英文 | 解释 |
| --- | --- | --- |
| 加速比 | Speedup | 串行执行时间与并行执行时间的比率，衡量并行化效果的指标 |
| 负载均衡 | Load Balancing | 将工作量合理分配给各处理单元，使它们的工作量大致相同 |
| 静态任务分配 | Static Task Assignment | 编译时或运行初始确定的任务分配方案，运行期间不再调整 |
| 动态任务分配 | Dynamic Task Assignment | 运行时根据实际负载情况动态调整任务分配的方案 |
| 图划分 | Graph Partitioning | 将图拆分为大小相近的子图，同时最小化跨越分区的边数 |
| 阿姆达尔定律 | Amdahl's Law | 描述并行化程序的最大可能加速比，强调串行部分的限制作用 |
| 并行开销 | Parallelism Overhead | 由于并行化引入的额外计算、通信和同步成本 |
| 坐标二分法 | Coordinate Bisection | 基于几何坐标划分图的方法，通常沿坐标轴中点切分 |
| 惯性二分法 | Inertial Bisection | 考虑点集分布特性的图划分方法，沿主方向进行切分 |
| 谱二分法 | Spectral Bisection | 基于拉普拉斯矩阵特征向量的图划分方法 |
| 多级方法 | Multilevel Methods | 通过粗化-划分-细化三个阶段高效处理大规模图划分的方法 |
| 工作窃取 | Work Stealing | 空闲线程从忙碌线程的任务队列中"窃取"任务的负载均衡技术 |
| 调度块大小 | Chunk Size | 在循环调度中一次分配给线程的连续迭代数量 |
| 引导调度 | Guided Scheduling | 块大小随着剩余迭代减少而自动减小的动态调度策略 |
| 粗粒度并行 | Coarse-grained Parallelism | 划分为较少、较大任务的并行方式，通信开销较小 |
| 细粒度并行 | Fine-grained Parallelism | 划分为众多小任务的并行方式，需要更多通信和同步 |