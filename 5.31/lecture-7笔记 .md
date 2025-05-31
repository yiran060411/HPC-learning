# lecture-7笔记

# **并行编程实践与OpenMP应用笔记**

## **1. 并行编程基础概念**

并行编程是指将一个大任务分解成若干子任务，让多个处理器或线程同时执行这些子任务，以提高整体计算速度的方法[[blog.csdn.net]](https://blog.csdn.net/shujian_tianya/article/details/104975474#:~:text=1)。传统的串行程序一次只能执行一个操作，而并行程序可以在同一时间执行多个操作，特别适合处理大规模数据或复杂计算。例如，在矩阵乘法中，结果矩阵的每个元素可以独立计算，因此可以将不同元素的计算分配给不同的线程同时进行[[cnblogs.com]](https://www.cnblogs.com/chihaoyuIsnotHere/p/10553617.html#:~:text=%E6%AD%A3%E5%A6%82%E5%89%8D%E9%9D%A2%E6%89%80%E8%AE%B2%E7%9A%84%EF%BC%8C%E7%9F%A9%E9%98%B5%E7%9B%B8%E4%B9%98%E8%BF%87%E7%A8%8B%E4%B8%AD%EF%BC%8C%E7%BB%93%E6%9E%9C%E7%9F%A9%E9%98%B5C%E4%B8%AD%E7%9A%84%E6%AF%8F%E4%B8%AA%E5%85%83%E7%B4%A0%E9%83%BD%E6%98%AF%E5%8F%AF%E4%BB%A5%E7%8B%AC%E7%AB%8B%E8%AE%A1%E7%AE%97%E7%9A%84%EF%BC%8C%E5%8D%B3%E5%BD%BC%E6%AD%A4%E4%B9%8B%E9%97%B4%E5%B9%B6%E6%97%A0%E4%BE%9D%E8%B5%96%E6%80%A7%E3%80%82%E6%89%80%E4%BB%A5%E5%A6%82%E6%9E%9C%E9%87%87%E7%94%A8%E6%9B%B4%E5%A4%9A%E7%9A%84%E5%A4%84%E7%90%86%E5%99%A8%EF%BC%8C%E5%B0%86%E4%BC%9A%E6%98%BE%E8%91%97%E5%9C%B0%E6%8F%90%E9%AB%98%E7%9F%A9%E9%98%B5%E7%9B%B8%E4%B9%98%E7%9A%84%E8%AE%A1%E7%AE%97%E6%95%88%E7%8E%87%E3%80%82)。

并行计算的关键指标之一是**加速比**（Speedup），它表示并行程序相对于串行程序的性能提升程度。加速比的计算公式为：

**Sp = T₁ / Tp**

其中，T₁是串行执行时间，Tp是使用p个处理器或线程的并行执行时间[[zhuanlan.zhihu.com]](https://zhuanlan.zhihu.com/p/658581660#:~:text=%E5%8A%A0%E9%80%9F%E6%AF%94%20%E5%8A%A0%E9%80%9F%E6%AF%94%EF%BC%88speedup%EF%BC%89%EF%BC%8C%E6%98%AF%E5%90%8C%E4%B8%80%E4%B8%AA%E4%BB%BB%E5%8A%A1%E5%9C%A8%E5%8D%95%E5%A4%84%E7%90%86%E5%99%A8%E7%B3%BB%E7%BB%9F%E5%92%8C%E5%B9%B6%E8%A1%8C%E5%A4%84%E7%90%86%E5%99%A8%E7%B3%BB%E7%BB%9F%E4%B8%AD%E8%BF%90%E8%A1%8C%E6%B6%88%E8%80%97%E7%9A%84%E6%97%B6%E9%97%B4%E7%9A%84%E6%AF%94%E7%8E%87%EF%BC%8C%E7%94%A8%E6%9D%A5%E8%A1%A1%E9%87%8F%E5%B9%B6%E8%A1%8C%E7%B3%BB%E7%BB%9F%E6%88%96%E7%A8%8B%E5%BA%8F%E5%B9%B6%E8%A1%8C%E5%8C%96%E7%9A%84%E6%80%A7%E8%83%BD%E5%92%8C%E6%95%88%E6%9E%9C%E3%80%82%20Sp=T1/Tp%20Sp%E6%98%AF%E5%8A%A0%E9%80%9F%E6%AF%94%EF%BC%8CT1%E6%98%AF%E5%8D%95%E5%A4%84%E7%90%86%E5%99%A8%E4%B8%8B%E7%9A%84%E8%BF%90%E8%A1%8C%E6%97%B6%E9%97%B4%EF%BC%8CTp%E6%98%AF%E5%9C%A8%E6%9C%89P%E4%B8%AA%E5%A4%84%E7%90%86%E5%99%A8%E2%80%A6)。例如，如果一个任务串行执行需要100秒，而使用4个线程并行执行需要30秒，则加速比约为3.33。理想情况下，增加p个处理器可以将时间降至1/p，即线性加速比Sp=p。但实际中由于任务划分 overhead、通信和同步开销，加速比往往小于p。

并行计算的主要挑战包括：

- **任务划分**：如何将问题分解为多个可并行执行的子任务。
- **负载均衡**：确保各处理器/线程的工作量大致相等，避免某些线程闲置。
- **同步与通信**：并行执行的线程或进程往往需要协调进度和交换数据，同步不当会导致性能下降甚至错误（如竞态条件）。
- **数据依赖**：某些计算步骤必须等待其他步骤完成（例如矩阵乘法中某行的计算可能依赖前面行的结果），这会限制并行性。

为了应对这些挑战，人们提出了多种**并行编程模型**和工具。常见的模型包括：

- **共享内存模型**：多个线程共享同一块内存空间，可以直接读写共享变量来交换数据，通常通过互斥锁、信号量等机制进行同步。OpenMP就是一种常用的共享内存编程API。
- **消息传递模型**：多个进程各自拥有独立的内存空间，通过发送和接收消息来交换数据，MPI是最流行的消息传递库。
- **数据并行模型**：将数据划分到不同处理器，各处理器对本地数据执行相同的操作（如SIMD指令或GPU的线程级并行）。
- **任务并行模型**：将不同的任务分配给不同的处理器，各任务可以处理不同的数据（例如多线程服务器处理不同请求）。

在并行编程中，还需注意**阿姆达尔定律**（Amdahl's Law）和**古斯塔夫森定律**（Gustafson's Law）等理论指导。阿姆达尔定律指出，程序中无法并行化的部分会限制整体的最大加速比[[en.wikipedia.org]](https://en.wikipedia.org/wiki/Amdahl's_law#:~:text=For%20instance,%20if%20a%20programmer,4)。例如，若有5%的代码只能串行执行，那么无论使用多少处理器，加速比最多只能接近20倍。古斯塔夫森定律则强调，当问题规模随处理器数量增加而扩大时，可并行部分的比例也会增加，从而可以获得更高的加速比。这些理论帮助我们在设计并行算法时权衡串行和并行部分的比例。

## **2. OpenMP基础与常用指令**

**OpenMP**（Open Multi-Processing）是一种用于共享内存并行编程的API，支持C/C++和Fortran语言[[w-jin.github.io]](https://w-jin.github.io/tech/openmp/#:~:text=OpenMP%E6%95%99%E7%A8%8B)。它通过编译器指令（以`#pragma omp`开头的编译指示）和库函数，为程序员提供了一种高层抽象来编写并行程序，无需直接操作复杂的线程创建和同步原语[[blog.csdn.net]](https://blog.csdn.net/ArrowYL/article/details/81094837#:~:text=OpenMP%E6%8F%90%E4%BE%9B%E4%BA%86%E5%AF%B9%E5%B9%B6%E8%A1%8C%E7%AE%97%E6%B3%95%E7%9A%84%E9%AB%98%E5%B1%82%E6%8A%BD%E8%B1%A1%E6%8F%8F%E8%BF%B0%EF%BC%8C%E7%89%B9%E5%88%AB%E9%80%82%E5%90%88%E5%9C%A8%E5%A4%9A%E6%A0%B8CPU%E6%9C%BA%E5%99%A8%E4%B8%8A%E7%9A%84%E5%B9%B6%E8%A1%8C%E7%A8%8B%E5%BA%8F%E8%AE%BE%E8%AE%A1%E3%80%82%E7%BC%96%E8%AF%91%E5%99%A8%E6%A0%B9%E6%8D%AE%E7%A8%8B%E5%BA%8F%E4%B8%AD%E6%B7%BB%E5%8A%A0%E7%9A%84pragma%E6%8C%87%E4%BB%A4%EF%BC%8C%E8%87%AA%E5%8A%A8%E5%B0%86%E7%A8%8B%E5%BA%8F%E5%B9%B6%E8%A1%8C%E5%A4%84%E7%90%86%EF%BC%8C%E4%BD%BF%E7%94%A8OpenMP%E9%99%8D%E4%BD%8E%E4%BA%86%E5%B9%B6%E8%A1%8C%E7%BC%96%E7%A8%8B%E7%9A%84%E9%9A%BE%E5%BA%A6%E5%92%8C%E5%A4%8D%E6%9D%82%E5%BA%A6%E3%80%82%E5%BD%93%E7%BC%96%E8%AF%91%E5%99%A8%E4%B8%8D%E6%94%AF%E6%8C%81OpenMP%E6%97%B6%EF%BC%8C%E7%A8%8B%E5%BA%8F%E4%BC%9A%E9%80%80%E5%8C%96%E6%88%90%E6%99%AE%E9%80%9A%EF%BC%88%E4%B8%B2%E8%A1%8C%EF%BC%89%E7%A8%8B%E5%BA%8F%E3%80%82)。使用OpenMP，程序员可以很容易地将串行代码中的循环、任务等并行化，编译器会自动生成多线程代码。如果编译器不支持OpenMP，代码会退化为串行执行，这保证了良好的兼容性[[blog.csdn.net]](https://blog.csdn.net/ArrowYL/article/details/81094837#:~:text=OpenMP%E6%8F%90%E4%BE%9B%E4%BA%86%E5%AF%B9%E5%B9%B6%E8%A1%8C%E7%AE%97%E6%B3%95%E7%9A%84%E9%AB%98%E5%B1%82%E6%8A%BD%E8%B1%A1%E6%8F%8F%E8%BF%B0%EF%BC%8C%E7%89%B9%E5%88%AB%E9%80%82%E5%90%88%E5%9C%A8%E5%A4%9A%E6%A0%B8CPU%E6%9C%BA%E5%99%A8%E4%B8%8A%E7%9A%84%E5%B9%B6%E8%A1%8C%E7%A8%8B%E5%BA%8F%E8%AE%BE%E8%AE%A1%E3%80%82%E7%BC%96%E8%AF%91%E5%99%A8%E6%A0%B9%E6%8D%AE%E7%A8%8B%E5%BA%8F%E4%B8%AD%E6%B7%BB%E5%8A%A0%E7%9A%84pragma%E6%8C%87%E4%BB%A4%EF%BC%8C%E8%87%AA%E5%8A%A8%E5%B0%86%E7%A8%8B%E5%BA%8F%E5%B9%B6%E8%A1%8C%E5%A4%84%E7%90%86%EF%BC%8C%E4%BD%BF%E7%94%A8OpenMP%E9%99%8D%E4%BD%8E%E4%BA%86%E5%B9%B6%E8%A1%8C%E7%BC%96%E7%A8%8B%E7%9A%84%E9%9A%BE%E5%BA%A6%E5%92%8C%E5%A4%8D%E6%9D%82%E5%BA%A6%E3%80%82%E5%BD%93%E7%BC%96%E8%AF%91%E5%99%A8%E4%B8%8D%E6%94%AF%E6%8C%81OpenMP%E6%97%B6%EF%BC%8C%E7%A8%8B%E5%BA%8F%E4%BC%9A%E9%80%80%E5%8C%96%E6%88%90%E6%99%AE%E9%80%9A%EF%BC%88%E4%B8%B2%E8%A1%8C%EF%BC%89%E7%A8%8B%E5%BA%8F%E3%80%82)。

OpenMP的核心思想是**fork-join**模型：程序开始时只有主线程（称为master线程），当遇到并行区域（parallel region）时，主线程会创建一个线程团队（team of threads），团队中的多个线程并行执行并行区域内的代码；当并行区域结束时，除了主线程外的其他线程终止（join），程序回到单线程执行。

下面介绍OpenMP中常用的指令和子句：

- **`#pragma omp parallel`**：用于创建一个并行区域，其后的代码块将由多个线程并行执行。例如：上述代码会启动多个线程（数量由OpenMP运行时决定，默认通常等于CPU核心数），每个线程都会打印自己的线程号。`omp_get_thread_num()`是OpenMP提供的库函数，用于获取当前线程的编号（主线程编号为0）。
    
    ```cpp
    #pragma omp parallel
    {
        // 并行执行的代码
        printf("Hello from thread %d\n", omp_get_thread_num());
    }
    
    ```
    
- **`#pragma omp for`**：用于并行化一个`for`循环，将循环迭代分配给不同的线程执行。该指令必须出现在`#pragma omp parallel`所限定的并行区域内，或者与`parallel`合并使用（如`#pragma omp parallel for`）。例如：编译器会将循环的迭代划分到各个线程中执行。默认情况下，循环迭代是平均分配的（静态划分）。可以通过`schedule`子句指定不同的调度策略，例如动态调度（`schedule(dynamic)`）用于迭代执行时间不均衡的情况。
    
    ```cpp
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        // 处理第i个元素
        compute(i);
    }
    
    ```
    
- **`#pragma omp sections`**：用于将多个**独立的代码段**分配给不同的线程执行。每个代码段以`#pragma omp section`开头。例如：上述代码中，`taskA`和`taskB`会被不同的线程并行执行。`sections`结构适用于处理几个**大块的独立任务**，而不是循环的情况。需要注意，`sections`必须放在`parallel`区域内，且每个`section`内的代码只会被执行一次。
    
    ```cpp
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            // 任务A，由某个线程执行
            taskA();
        }
        #pragma omp section
        {
            // 任务B，由另一个线程执行
            taskB();
        }
    }
    
    ```
    
- **`#pragma omp single`**：指定一段代码只由**一个线程**执行（通常是主线程），其他线程跳过该段代码。常用于并行区域内的初始化或清理工作。例如：这里的初始化操作`initialize_data()`只在一个线程中执行，避免多线程重复初始化。而`process_data()`则会被所有线程并行执行。
    
    ```cpp
    #pragma omp parallel
    {
        #pragma omp single
        {
            printf("Initializing data...\n"); // 仅由一个线程打印
            initialize_data();
        }
        // 所有线程都会执行下面的处理
        process_data();
    }
    
    ```
    
- **`#pragma omp master`**：与`single`类似，指定代码块由主线程（master线程，即线程号为0的线程）执行。与`single`不同的是，如果主线程在执行`master`块时其他线程可能已经到达块后的同步点，而`single`块执行完毕后会等待所有线程到达才继续。通常`master`用于不需要等待其他线程的场合。
- **`#pragma omp critical`**：用于定义一个**临界区**，确保同一时间只有一个线程进入该代码块，防止多个线程同时修改共享数据导致竞态条件。例如：上述代码中，`counter++`操作被包裹在`critical`块中，确保每次只有一个线程执行该操作，从而正确地对计数器进行递增。临界区会带来一定的性能开销，应尽量缩小其范围。
    
    ```cpp
    int counter = 0;
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        // ... 其他并行操作 ...
        #pragma omp critical
        {
            counter++; // 临界区，保证原子性
        }
    }
    
    ```
    
- **`#pragma omp atomic`**：用于对一个变量进行**原子操作**，保证该操作的不可分割性。它比`critical`更轻量，适用于简单的读写修改操作。例如：`atomic`指令确保`counter++`作为一个原子步骤完成，避免竞态条件。支持的原子操作包括赋值、加减、逻辑运算等，但要求操作是简单的单一表达式。
    
    ```cpp
    #pragma omp atomic
    counter++; // 等价于在critical中执行counter++，但更高效
    
    ```
    
- **`#pragma omp barrier`**：设置一个**屏障**，所有线程执行到此处时必须等待，直到团队中所有线程都到达该点后才继续执行后续代码。屏障用于同步并行线程的进度。例如：这里，所有线程在完成`do_some_work`后，必须在屏障处等待，直到最后一个线程完成`do_some_work`，然后一起执行`do_rest_of_work`。
    
    ```cpp
    #pragma omp parallel
    {
        do_some_work();
        #pragma omp barrier         // 所有线程完成do_some_work后才继续
        do_rest_of_work();
    }
    
    ```
    
- **`#pragma omp reduction`**：用于在并行区域中对一个变量进行**归约操作**（如求和、求最大值等）。OpenMP会为每个线程创建该变量的私有副本，并行执行结束后自动将各副本的结果合并到原变量。例如：上述代码中，`reduction(+:sum)`表示对变量`sum`进行加法归约。每个线程会有自己的`sum`局部变量，循环结束后各线程的局部和会相加得到最终的总和。归约操作避免了显式的临界区同步，是并行求和、求积等操作的高效方法。
    
    ```cpp
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; ++i) {
        sum += array[i]; // 每个线程累加自己的副本，最后合并到sum
    }
    
    ```
    
- **`#pragma omp task`**：用于创建**任务**（task），这是OpenMP 3.0引入的重要特性，用于支持更灵活的并行模式（如动态任务分配、递归并行等）。使用`task`指令可以将一段代码封装成一个任务，并将其加入任务队列，由线程团队中的线程异步执行。例如：上述代码中，主线程（通过`single`指令）生成N个任务，每个任务处理一个元素。这些任务会被线程团队中的线程**按需获取并执行**。`task`机制特别适合处理迭代次数不确定的循环（如链表遍历）或递归结构，它可以将工作动态地分配给空闲线程，实现负载均衡。
    
    ```cpp
    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int i = 0; i < N; ++i) {
                #pragma omp task
                {
                    process_item(i); // 每个迭代生成一个任务
                }
            }
        }
    }
    
    ```
    

除了上述指令，OpenMP还提供了许多**子句（clauses）**用于控制数据共享属性和并行行为。例如：

- `private(list)`：指定某些变量在每个线程中拥有私有副本，各线程对该变量的修改不会互相干扰。
- `firstprivate(list)`：类似`private`，但线程的私有变量会用原始变量的初始值初始化。
- `lastprivate(list)`：在线程并行结束后，将某线程的私有变量值复制回原始变量（通常用于循环的最后一次迭代值需要保留的情况）。
- `shared(list)`：显式指定某些变量为所有线程共享（默认情况下，在并行区域外定义的变量在内部是共享的）。
- `num_threads(n)`：指定并行区域中使用的线程数量。
- `nowait`：取消某些指令隐含的屏障（例如用在`for`或`sections`后，使线程执行完该结构后不必等待其他线程）。

通过组合使用这些指令和子句，可以编写出高效且正确的并行程序。需要注意的是，OpenMP要求程序员自己确保共享数据的访问是安全的（例如使用临界区或原子操作保护共享变量），编译器不会自动检测和处理数据竞争[[cs.hunter.cuny.edu]](https://www.cs.hunter.cuny.edu/~sweiss/course_materials/csci493.65/lecture_notes/chapter11.pdf#:~:text=%E2%80%A2%20is%20not%20required%20to,%E2%80%A2%20does%20not%20guarantee%20that)。因此，在编写OpenMP程序时，需要仔细分析共享变量的访问模式，避免竞态条件等错误。

## **3. OpenMP的sections和task构造的代码示例**

为了更直观地理解OpenMP的`sections`和`task`构造的用法，下面给出具体的代码示例。

### **3.1 sections构造的使用示例**

`sections`构造用于将程序中的多个独立代码段并行执行。每个`section`内的代码由一个线程执行，不同`section`的代码由不同线程同时执行。以下是一个简单的例子：

```cpp
#include <stdio.h>
#include <omp.h>

void taskA() { printf("Task A executed by thread %d\n", omp_get_thread_num()); }
void taskB() { printf("Task B executed by thread %d\n", omp_get_thread_num()); }
void taskC() { printf("Task C executed by thread %d\n", omp_get_thread_num()); }

int main() {
    #pragma omp parallel sections num_threads(3)
    {
        #pragma omp section
        taskA();  // 由线程1执行（假设线程编号从0开始）
        #pragma omp section
        taskB();  // 由线程2执行
        #pragma omp section
        taskC();  // 由线程3执行
    }
    return 0;
}

```

在这个示例中，`#pragma omp parallel sections`创建了一个并行区域，并指定使用3个线程。区域内有三个`section`块，分别调用`taskA`、`taskB`、`taskC`。运行时，这3个任务会被分配给3个线程同时执行。输出可能类似：

```
Task A executed by thread 0
Task B executed by thread 1
Task C executed by thread 2

```

（实际输出顺序可能因线程调度而不同，但三个任务一定是并行完成的。）

需要注意，`sections`结构中的每个`section`只会被执行一次，所有`section`执行完毕后，并行区域结束。如果`sections`没有`nowait`子句，线程在完成自己的section后会在隐式的屏障处等待，直到所有section执行完毕，然后一起退出并行区域[[blog.csdn.net]](https://blog.csdn.net/augusdi/article/details/8808158#:~:text=%E9%87%87%E7%94%A8section%E5%AE%9A%E4%B9%89%E7%9A%84%E6%AF%8F%E6%AE%B5%E7%A8%8B%E5%BA%8F%E9%83%BD%E5%B0%86%E5%8F%AA%E6%89%A7%E8%A1%8C%E4%B8%80%E6%AC%A1%EF%BC%8Csections%E4%B8%AD%E7%9A%84%E6%AF%8F%E6%AE%B5section%E5%B0%86%E5%B9%B6%E8%A1%8C%E6%89%A7%E8%A1%8C%E3%80%82%E4%B8%80%E4%B8%AA%E7%A8%8B%E5%BA%8F%E4%B8%AD%E5%8F%AF%E4%BB%A5%E5%AE%9A%E4%B9%89%E5%A4%9A%E4%B8%AAsections%EF%BC%8C%E6%AF%8F%E4%B8%AAsections%E4%B8%AD%E5%8F%88%E5%8F%AF%E4%BB%A5%E5%AE%9A%E4%B9%89%E5%A4%9A%E4%B8%AAsection%E3%80%82%E5%90%8C%E4%B8%80%E4%B8%AAsections%E4%B8%ADsection%E4%B9%8B%E9%97%B4%E5%A4%84%E4%BA%8E%E5%B9%B6%E8%A1%8C%E7%8A%B6%E6%80%81%E3%80%82sections%E4%B8%8E%E5%85%B6%E4%BB%96sections%E4%B9%8B%E9%97%B4%E5%A4%84%E4%BA%8E%E4%B8%B2%E8%A1%8C%E7%8A%B6%E6%80%81%E3%80%82)。如果有多个`sections`结构，它们之间是串行的，即第一个`sections`全部执行完后才会执行第二个`sections`[[blog.csdn.net]](https://blog.csdn.net/augusdi/article/details/8808158#:~:text=%E9%87%87%E7%94%A8section%E5%AE%9A%E4%B9%89%E7%9A%84%E6%AF%8F%E6%AE%B5%E7%A8%8B%E5%BA%8F%E9%83%BD%E5%B0%86%E5%8F%AA%E6%89%A7%E8%A1%8C%E4%B8%80%E6%AC%A1%EF%BC%8Csections%E4%B8%AD%E7%9A%84%E6%AF%8F%E6%AE%B5section%E5%B0%86%E5%B9%B6%E8%A1%8C%E6%89%A7%E8%A1%8C%E3%80%82%E4%B8%80%E4%B8%AA%E7%A8%8B%E5%BA%8F%E4%B8%AD%E5%8F%AF%E4%BB%A5%E5%AE%9A%E4%B9%89%E5%A4%9A%E4%B8%AAsections%EF%BC%8C%E6%AF%8F%E4%B8%AAsections%E4%B8%AD%E5%8F%88%E5%8F%AF%E4%BB%A5%E5%AE%9A%E4%B9%89%E5%A4%9A%E4%B8%AAsection%E3%80%82%E5%90%8C%E4%B8%80%E4%B8%AAsections%E4%B8%ADsection%E4%B9%8B%E9%97%B4%E5%A4%84%E4%BA%8E%E5%B9%B6%E8%A1%8C%E7%8A%B6%E6%80%81%E3%80%82sections%E4%B8%8E%E5%85%B6%E4%BB%96sections%E4%B9%8B%E9%97%B4%E5%A4%84%E4%BA%8E%E4%B8%B2%E8%A1%8C%E7%8A%B6%E6%80%81%E3%80%82)。例如：

```cpp
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section { /* 任务1 */ }
        #pragma omp section { /* 任务2 */ }
    } // 所有线程在此等待，直到任务1和任务2完成
    #pragma omp sections
    {
        #pragma omp section { /* 任务3 */ }
        #pragma omp section { /* 任务4 */ }
    } // 所有线程在此等待，直到任务3和任务4完成
}

```

在这个例子中，任务1和任务2并行执行，完成后所有线程继续执行后面的第二个sections，任务3和任务4并行执行。两个sections结构之间是串行的，即任务3和任务4必须等到任务1和任务2完成后才会开始。

`sections`构造适用于**任务级并行**的场景，例如程序中有几个相对独立的部分需要同时运行。比如，一个数据处理程序可能同时需要读取输入、处理数据、输出结果，这三个步骤如果彼此独立，就可以放在不同的section中并行执行。不过实际中更常见的是用`sections`来并行化非循环的代码块，而循环的并行化则更多使用`for`指令。

### **3.2 task构造的使用示例**

`task`构造用于动态地生成任务，这些任务可以被线程团队中的线程异步执行。这在处理具有不规则并行结构或递归的问题时非常有用。下面通过两个例子来说明。

**示例1：动态任务生成（链表遍历）**

考虑一个链表遍历的场景，每个节点需要被处理，但节点数量和结构在编译时未知。使用OpenMP的`task`可以方便地实现并行处理：

```cpp
#include <omp.h>

struct Node {
    int data;
    Node* next;
};

void process(Node* node) {
    // 处理该节点的操作，例如打印数据
    printf("Processing node %d\n", node->data);
}

void traverse(Node* head) {
    #pragma omp parallel
    {
        #pragma omp single
        {
            Node* current = head;
            while (current != nullptr) {
                #pragma omp task firstprivate(current)
                {
                    process(current);
                }
                current = current->next;
            }
        }
    }
}

```

在`traverse`函数中，我们首先进入一个并行区域，然后通过`single`指令让主线程负责遍历链表。对于每个节点，主线程生成一个新的任务（`#pragma omp task`），并将当前节点指针`current`作为`firstprivate`变量传递给任务（确保每个任务处理自己的节点副本）。每个任务调用`process`函数处理对应的节点。这些任务会被放入任务队列，由线程团队中的线程**并行地执行**。最终的效果是，链表的所有节点被多个线程并发处理，而无需提前知道节点数量。

需要注意，`task`指令必须在并行区域内使用，通常配合`single`或`master`使用以避免重复生成任务。上述例子中如果不用`single`，每个线程都会遍历链表并生成所有任务，导致每个节点被处理多次。因此用`single`保证只有主线程生成任务。

**示例2：递归任务（斐波那契数列）**

`task`构造还可以用于实现递归的并行算法。例如，计算斐波那契数列的递归版本可以改写成并行形式：

```cpp
#include <stdio.h>
#include <omp.h>

int fib(int n) {
    if (n < 2) return n;
    int i, j;
    #pragma omp task shared(i)
    i = fib(n - 1);
    #pragma omp task shared(j)
    j = fib(n - 2);
    #pragma omp taskwait
    return i + j;
}

int main() {
    omp_set_num_threads(4); // 设置最大线程数
    #pragma omp parallel
    {
        #pragma omp single
        {
            int result = fib(20); // 在并行区域内调用fib，生成任务
            printf("fib(20) = %d\n", result);
        }
    }
    return 0;
}

```

在这个并行的斐波那契计算中，每次递归调用会生成两个新的任务，分别计算`fib(n-1)`和`fib(n-2)`。`#pragma omp taskwait`指令会让当前线程等待它所生成的所有子任务完成后再继续执行（这里即等待i和j的计算结果）。这样，整个递归过程被分解成大量的小任务，由多个线程并行执行。当n较大时，这可以显著加快计算速度（当然，斐波那契数列的递归实现效率并不高，这里仅用来说明任务并行的思想）。

使用`task`构造时，需要注意任务之间的依赖关系。例如上面的例子中，父任务需要等待子任务的结果，因此用`taskwait`同步。如果任务之间没有依赖，可以完全并发执行。此外，过多的任务可能带来调度开销，因此需要根据实际情况控制任务粒度。通常每个任务应包含足够多的计算量，避免任务数量过多而每个任务执行时间太短。

总的来说，`sections`和`task`都是OpenMP中实现任务并行的手段。`sections`适合明确知道几个独立任务的情况，而`task`适合动态生成不确定数量任务的情况。合理使用它们可以充分发挥多核CPU的并行能力，提高程序效率。

## **4. 分布式内存平台下的矩阵乘法算法分析**

前面介绍的OpenMP编程适用于**共享内存**环境（如单机多核）。然而，对于大规模计算问题，仅靠单个机器的内存和计算能力可能不够，这时需要使用多台机器组成的集群，各机器通过网络连接，形成**分布式内存**环境。在分布式内存平台上，每个节点（机器）有自己独立的内存，进程之间不能直接访问彼此的内存，必须通过消息传递（如MPI）来交换数据[[cnblogs.com]](https://www.cnblogs.com/focusonoutput/p/12439752.html#:~:text=%E5%85%B1%E4%BA%AB%E5%86%85%E5%AD%98Distributed%20Memory%20%E4%B8%8E%E5%88%86%E5%B8%83%E5%BC%8F%E5%86%85%E5%AD%98Distributed%20Memory%20%E6%88%91%E4%BB%AC%E7%BB%8F%E5%B8%B8%E8%AF%B4%E5%88%B0%E7%9A%84%E5%A4%9A%E6%A0%B8%E5%A4%84%E7%90%86%E5%99%A8%EF%BC%8C%E6%98%AF%E6%8C%87%E4%B8%80%E4%B8%AA%E5%A4%84%E7%90%86%E5%99%A8%EF%BC%88CPU%EF%BC%89%E4%B8%8A%E6%9C%89%E5%A4%9A%E4%B8%AA%E5%A4%84%E7%90%86%E6%A0%B8%E5%BF%83%EF%BC%88CORE%EF%BC%89%EF%BC%8C%E5%85%B1%E4%BA%AB%E5%86%85%E5%AD%98%E5%A4%9A%E6%A0%B8%E7%B3%BB%E7%BB%9F%E6%88%91%E4%BB%AC%E5%8F%AF%E4%BB%A5%E5%B0%86CPU%E6%83%B3%E8%B1%A1%E4%B8%BA%E4%B8%80%E4%B8%AA%E5%AF%86%E5%B0%81%E7%9A%84%E5%8C%85%EF%BC%8C%E5%9C%A8%E8%BF%99%E4%B8%AA%E5%8C%85%E5%86%85%E6%9C%89%E5%A4%9A%E4%B8%AA%E4%BA%92%E7%9B%B8%E8%BF%9E%E6%8E%A5%E7%9A%84CORES%EF%BC%8C%E6%AF%8F%E4%B8%AACORE%E5%85%B1%E4%BA%AB%E4%B8%80%E4%B8%AA%E4%B8%BB%E5%AD%98%EF%BC%8C%E6%89%80%E6%9C%89%E7%9A%84%E5%A4%84%E7%90%86%E6%A0%B8%E5%BF%83%E9%83%BD%E5%8F%AF%E4%BB%A5%E8%AE%BF%E9%97%AE%E4%B8%BB%E5%AD%98%E3%80%82%20%E5%88%86%E5%B8%83%E5%BC%8F%E5%86%85%E5%AD%98%E7%B3%BB%E7%BB%9F%E6%98%AF%E7%94%B1%E5%A4%9A%E4%B8%AA%E5%A4%84%E7%90%86%E5%99%A8)。因此，分布式内存并行编程的关键在于如何划分数据、分配任务，并设计高效的通信策略。

矩阵乘法是科学计算中最常见的核心运算之一，也是许多并行算法研究的对象。下面我们分析在分布式内存平台下，如何并行化矩阵乘法，并比较不同的并行策略。

### **4.1 矩阵乘法的并行性分析**

给定两个矩阵A和B，计算它们的乘积C=A×B，其中A的大小为m×k，B的大小为k×n，结果C的大小为m×n。矩阵乘法的串行算法通常是三重循环：

```cpp
for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
        for (k = 0; k < k; k++)
            C[i][j] += A[i][k] * B[k][j];

```

矩阵乘法的一个重要性质是，结果矩阵C中的每个元素C[i][j]的计算只依赖于A的第i行和B的第j列，与其他元素的计算**相互独立**[[cnblogs.com]](https://www.cnblogs.com/chihaoyuIsnotHere/p/10553617.html#:~:text=%E6%AD%A3%E5%A6%82%E5%89%8D%E9%9D%A2%E6%89%80%E8%AE%B2%E7%9A%84%EF%BC%8C%E7%9F%A9%E9%98%B5%E7%9B%B8%E4%B9%98%E8%BF%87%E7%A8%8B%E4%B8%AD%EF%BC%8C%E7%BB%93%E6%9E%9C%E7%9F%A9%E9%98%B5C%E4%B8%AD%E7%9A%84%E6%AF%8F%E4%B8%AA%E5%85%83%E7%B4%A0%E9%83%BD%E6%98%AF%E5%8F%AF%E4%BB%A5%E7%8B%AC%E7%AB%8B%E8%AE%A1%E7%AE%97%E7%9A%84%EF%BC%8C%E5%8D%B3%E5%BD%BC%E6%AD%A4%E4%B9%8B%E9%97%B4%E5%B9%B6%E6%97%A0%E4%BE%9D%E8%B5%96%E6%80%A7%E3%80%82%E6%89%80%E4%BB%A5%E5%A6%82%E6%9E%9C%E9%87%87%E7%94%A8%E6%9B%B4%E5%A4%9A%E7%9A%84%E5%A4%84%E7%90%86%E5%99%A8%EF%BC%8C%E5%B0%86%E4%BC%9A%E6%98%BE%E8%91%97%E5%9C%B0%E6%8F%90%E9%AB%98%E7%9F%A9%E9%98%B5%E7%9B%B8%E4%B9%98%E7%9A%84%E8%AE%A1%E7%AE%97%E6%95%88%E7%8E%87%E3%80%82)。也就是说，各个C[i][j]的计算可以并行进行。这为并行化提供了天然的条件。理论上，如果有足够多的计算资源，可以将每个C[i][j]的计算分配给不同的处理器同时计算。然而，在实际的并行实现中，我们需要考虑如何划分矩阵和平衡通信与计算的开销。

在分布式内存环境下，通常的做法是将大矩阵划分成若干子块，分配给不同的进程。每个进程持有矩阵的一部分，计算结果的一部分，并在需要的时候与其他进程通信以获取所需的数据。根据划分方式的不同，主要有以下几种并行矩阵乘法策略：

- **按行划分（1D行划分）**：将矩阵A按行分成若干块，每个进程负责计算结果矩阵C的对应行块。例如，若有p个进程，可将A的m行分成p块，每个进程得到m/p行，同时每个进程需要持有完整的矩阵B（或者B的相应部分）。这样每个进程计算C的m/p行，最后将各自行块合并得到完整的C。这种方法实现简单，但每个进程需要存储完整的B矩阵，内存占用较大，且当p很大时，B的广播会带来较高的通信开销。
- **按列划分（1D列划分）**：类似地，将矩阵B按列划分，每个进程负责计算结果矩阵C的对应列块。每个进程持有完整的A矩阵和B的一部分列，计算C的一部分列。同样存在内存和通信问题。
- **二维分块（2D分块）**：将矩阵A和B都划分成p个块（例如，当p是平方数时，排成√p×√p的网格），每个进程持有A的一个子块和B的一个子块，负责计算C的一个子块。通过在进程间循环传递子块数据，每个进程可以累积计算出C的子块。这种方法可以更好地平衡负载，并减少通信量，是分布式矩阵乘法中常用的策略（如Cannon算法和Fox算法）。
- **其他方法**：如三维分块、并行Strassen算法等，但实际中前两种方法更为常见。

下面重点介绍两种经典的分布式矩阵乘法算法：**Cannon算法**和**SUMMA（Scalable Universal Matrix Multiplication Algorithm）**，并对它们的通信开销进行分析比较。

### **4.2 Cannon算法**

Cannon算法是一种适用于二维网格结构的分布式矩阵乘法算法，由Lynn Cannon于1969年提出[[en.wikipedia.org]](https://en.wikipedia.org/wiki/Cannon's_algorithm#:~:text=In%20computer%20science,%20Cannon's%20algorithm,2)。它的基本思想是将参与运算的进程排列成一个二维的网格（p = q×q个进程排成q×q的方阵），并将矩阵A和B分块成q×q个子块，每个进程持有A的一个子块A_ij和B的一个子块B_ij，负责计算结果C的子块C_ij。算法通过循环移位A和B的子块，使得每个进程在每一步都能用本地的A子块和B子块相乘，并将结果累加到C的对应子块中。

**算法步骤：**

1. **初始移位**：为了使对应的子块对齐，初始时需要对A和B的子块进行循环移位。具体来说，第i行的进程将其A子块向左循环移动i步，第j列的进程将其B子块向上循环移动j步。这样，每个进程(i,j)在第一步将拥有A的子块A_i,(j+i) mod q和B的子块B_(i+j) mod q,j，从而可以正确计算C_ij的一部分。
2. **迭代计算**：在接下来的q次迭代中，每个进程执行以下操作：
    - 将本地的A子块向左循环移动一位（同一行内的进程之间传递）。
    - 将本地的B子块向上循环移动一位（同一列内的进程之间传递）。
    - 将当前的A子块和B子块相乘，结果累加到C的子块C_ij中。
3. 经过q次迭代后，每个进程完成了C_ij = Σ A_ik × B_kj (k=0到q-1)的计算，即得到了完整的C_ij子块。所有进程的C子块组合起来就得到最终的矩阵C。

**通信分析：**

Cannon算法的每次迭代中，每个进程需要发送和接收各一个A子块和一个B子块。假设子块的大小为n/q × n/q（假设原矩阵是n×n，分成q×q块），则每次通信的数据量约为2*(n/q)²（每个子块的大小）。总共进行q次迭代，因此每个进程的总通信数据量约为2q*(n/q)² = 2n²/q。如果有p=q²个进程，总通信量可表示为2n²/√p。Cannon算法主要的通信开销是每次迭代的**近邻通信**（左移和上移），这可以在二维网格上高效实现，通常不会产生网络瓶颈。

Cannon算法的优势在于其**存储效率**：每个进程只需存储A和B的一个子块以及C的一个子块，内存占用是O((n/q)²)，与进程数无关[[en.wikipedia.org]](https://en.wikipedia.org/wiki/Cannon's_algorithm#:~:text=The%20main%20advantage%20of%20the,number%20of%20processors.)。这使得它在大规模并行时内存使用非常高效。此外，Cannon算法通过循环移位将通信分散到各次迭代，使得计算和通信可以较好地重叠，提高了并行效率。

然而，Cannon算法要求进程数为平方数，并且需要严格的同步（每一步的移位和计算需要所有进程协调一致）。当进程数无法构成方阵时，需要调整算法或引入虚拟进程。此外，如果矩阵尺寸不能被q整除，子块大小会不一致，需要特殊处理。

### **4.3 SUMMA算法**

SUMMA（可扩展的通用矩阵乘法算法）是一种更通用的分布式矩阵乘法方法，由Van de Geijn和Watts于1995年提出[[github.com]](https://github.com/const-sambird/summa#:~:text=This%20repository%20benchmarks%20the%20performance,repository,%20report)。SUMMA并不限定具体的进程拓扑，而是从抽象的角度将矩阵乘法分解为一系列的外积更新，并通过集体通信在进程间分配计算任务。其核心思想是：将矩阵A和B划分到p个进程上，每个进程负责计算结果矩阵C的一部分，通过广播A的块和B的块，各进程并行地进行矩阵块相乘并累加结果。

**算法步骤：**

假设将矩阵A按行划分为p块（A_0, A_1, ..., A_{p-1}），矩阵B按列划分为p块（B_0, B_1, ..., B_{p-1}），每个进程i持有A_i和B_i，并负责计算C的一部分（比如C_i）。SUMMA的计算过程可以看作是一系列的秩-one更新（外积）：

C = Σ (A_k × B_k), 其中k从0到p-1。

具体来说，在每一步k，进程k将它持有的A_k广播给所有进程（或者广播给相关的进程组），同时将B_k也广播给所有进程。然后，每个进程用接收到的A_k和B_k相乘，得到一个外积矩阵，并将这个外积加到自己负责的C部分上。经过p步这样的操作后，所有进程完成各自C部分的计算。

上述描述是SUMMA的一种简化情况。实际中，SUMMA允许更灵活的划分方式（例如二维分块），并且可以采用不同的通信策略（广播或全收集等）。例如，在二维进程网格的情况下，SUMMA可以让每个进程持有A的一个子块和B的一个子块，然后在每一步k，第k行的进程广播其A子块到所在行，第k列的进程广播其B子块到所在列，这样每个进程(i,j)可以获取到A_ik和B_kj，相乘得到外积并累加到C_ij中。这与Cannon算法的思想有相似之处，但SUMMA更强调算法的通用性和可扩展性。

**通信分析：**

SUMMA的通信开销取决于采用的通信方式。假设使用广播策略，在每一步k，需要广播A的一个块和B的一个块。如果有p个进程，广播一个大小为(n/p)×n的块（假设A是n×n，按行分块）的时间大约是O(α + β n²/p)（α是启动时间，β是网络带宽的倒数）。进行p步这样的广播，总的通信时间约为p*(α + β n²/p) = pα + β n²。如果采用更高效的集体通信（如全收集或归约），通信复杂度可能略有不同，但总体量级相似。对于二维分块的情况，假设进程排成q×q网格，每一步广播行和列的子块，每步通信量约为2*(n/q)²，进行q步，总通信量约为2q*(n/q)² = 2n²/q，与Cannon算法相当。然而，SUMMA通常使用**树形广播**或**超立方广播**等方式，其通信时间可能包含对数因子（例如O(log p)的步数）。

与Cannon算法相比，SUMMA的优势在于其**灵活性和负载均衡**。SUMMA不要求进程数为平方数，也不要求矩阵尺寸能被进程数整除，它可以适应各种划分方式。此外，SUMMA的每一步计算都是全局的广播和相乘，负载在各进程间保持均衡。SUMMA的缺点是**通信开销可能较高**，因为每一步需要全局的广播，当进程数p很大时，广播的时间（尤其是启动时间α的累积）可能成为瓶颈。此外，SUMMA需要较多的内存，因为在某些实现中每个进程可能需要存储多个A和B的子块副本。

### **4.4 通信开销比较**

为了更直观地比较Cannon算法和SUMMA的通信开销，我们考虑在p个进程上计算n×n矩阵相乘的情况。假设p是一个平方数，q=√p，每个进程处理n/q × n/q的子块。

- **Cannon算法**：每个进程在q次迭代中，每次发送和接收各一个A子块和一个B子块。每个子块大小为(n/q)×(n/q)，即数据量为(n/q)²。因此每个进程的总通信数据量为：发送A子块 q次 × (n/q)² + 接收A子块 q次 × (n/q)² + 发送B子块 q次 × (n/q)² + 接收B子块 q次 × (n/q)² = 4q*(n/q)² = 4n²/q。考虑到每次移位可以同时发送和接收（双向通信），实际的数据传输总量可以减半，因此通常认为Cannon的总通信量约为2n²/q（每个进程发送和接收各n²/q的数据）。如果以p表示，q=√p，则总通信量约为2n²/√p。Cannon算法的通信模式是**近邻通信**，每次通信只涉及相邻的进程，因此通信延迟较低，且容易与计算重叠。
- **SUMMA算法**：假设采用广播方式，每个进程在p次迭代中，每次需要参与一次A块的广播和一次B块的广播。每次广播的数据量如果是按行划分的整个行块（n×n/p），则每个进程在每次广播中接收的数据量为n×n/p（假设自己不持有该块的话）。但更准确地说，在二维分块的SUMMA中，每一步广播的是子块(n/q)×(n/q)，每个进程需要接收一个A子块和一个B子块。进行q步的话，总接收数据量也是q*(n/q)² + q*(n/q)² = 2n²/q，和Cannon相当。然而，SUMMA的广播通常需要O(log q)步来完成一个子块在q个进程中的传播（例如使用二叉树广播），因此总的通信时间可能包含对数因子。此外，如果p不是平方数，SUMMA可能需要调整分块策略，通信开销会略有不同。

总的来说，Cannon和SUMMA在**渐近的通信复杂度**上是同一量级的，都是O(n²/√p)。然而，它们的**实际通信行为**有所不同：Cannon利用循环移位进行近邻通信，通信次数多但每次数据量小，且容易流水线化；SUMMA利用全局广播，通信次数相对少但每次涉及更多进程，启动开销可能更高。在实现中，SUMMA通常结合了一些优化，例如使用**环形广播**或**分层通信**来减少延迟[[hcl.ucd.ie]](https://hcl.ucd.ie/system/files/JoS%202014%20hierarchical%20matrix%20multiplication.pdf#:~:text=,the%20communication%20cost%20of%20SUMMA)。例如，在具有层次结构的集群中（计算节点内有多个核心），可以先在节点内广播，再在节点间广播，以减少全局通信的时间。

除了Cannon和SUMMA，还有一些其他的分布式矩阵乘法策略值得一提：

- **Fox算法**：另一种基于二维网格的算法，由Fox等人提出。它与Cannon类似，但采用主进程广播的方式来提供A的子块，而B的子块在列内循环移位。Fox算法的通信开销与Cannon相近，但需要更多的同步。
- **1.5D和2.5D算法**：这些是近年来提出的介于1D和2D之间的算法，通过增加少量的冗余存储来减少通信次数。例如，2.5D算法将矩阵分成三个维度，让每个结果块由多个进程共同计算，从而将通信复杂度进一步降低。这些算法在大规模并行时表现出更好的扩展性。
- **并行Strassen算法**：Strassen算法是一种时间复杂度略低于O(n³)的串行矩阵乘法算法（约O(n^2.807)）。其并行版本可以将递归分解的子问题分配给不同进程。然而，Strassen算法的并行实现较为复杂，且对矩阵尺寸和进程数有严格要求，实际中不如上述基于分块的算法常用。

在选择分布式矩阵乘法算法时，需要综合考虑**进程数量、矩阵大小、网络带宽**等因素。对于中等规模的进程数和规则的网格拓扑，Cannon算法是一个高效且易于实现的选择。对于更通用的场景或需要更好的扩展性，SUMMA提供了更大的灵活性，可以通过调整分块策略来优化性能。在极端大规模的情况下（例如数万个进程），可能需要采用更高级的算法（如2.5D）或者结合共享内存和分布式内存的混合编程（例如每个节点内用OpenMP多线程，节点间用MPI通信）来平衡计算和通信开销。

最后需要强调的是，分布式并行计算中，**通信往往是性能的瓶颈**。矩阵乘法的并行算法设计核心在于如何**减少通信量**和**提高计算/通信比**。例如，通过增大分块大小，可以减少通信次数，但会增加每个进程的计算量和内存占用；反之，减小分块大小会增加通信次数，但可能改善负载均衡。实际应用中需要根据具体情况调优。此外，利用缓存和局部性原理，在每次通信后尽可能多地进行计算（例如在接收到一块数据后完成其所有相关的乘加运算），也是提高并行效率的重要手段。

**注：** 专业术语解释：

- **并行编程**：将一个任务分解为多个子任务，由多个处理器或线程同时执行，以提高整体效率的编程方法[[blog.csdn.net]](https://blog.csdn.net/shujian_tianya/article/details/104975474#:~:text=1)。
- **OpenMP**：一种用于共享内存并行编程的API，通过编译器指令和库函数实现多线程并行，简化了并行程序的编写[[w-jin.github.io]](https://w-jin.github.io/tech/openmp/#:~:text=OpenMP%E6%95%99%E7%A8%8B)。
- **分布式内存**：多台计算机组成的并行系统，每台计算机有独立内存，进程间通过网络消息传递进行通信[[cnblogs.com]](https://www.cnblogs.com/focusonoutput/p/12439752.html#:~:text=%E5%85%B1%E4%BA%AB%E5%86%85%E5%AD%98Distributed%20Memory%20%E4%B8%8E%E5%88%86%E5%B8%83%E5%BC%8F%E5%86%85%E5%AD%98Distributed%20Memory%20%E6%88%91%E4%BB%AC%E7%BB%8F%E5%B8%B8%E8%AF%B4%E5%88%B0%E7%9A%84%E5%A4%9A%E6%A0%B8%E5%A4%84%E7%90%86%E5%99%A8%EF%BC%8C%E6%98%AF%E6%8C%87%E4%B8%80%E4%B8%AA%E5%A4%84%E7%90%86%E5%99%A8%EF%BC%88CPU%EF%BC%89%E4%B8%8A%E6%9C%89%E5%A4%9A%E4%B8%AA%E5%A4%84%E7%90%86%E6%A0%B8%E5%BF%83%EF%BC%88CORE%EF%BC%89%EF%BC%8C%E5%85%B1%E4%BA%AB%E5%86%85%E5%AD%98%E5%A4%9A%E6%A0%B8%E7%B3%BB%E7%BB%9F%E6%88%91%E4%BB%AC%E5%8F%AF%E4%BB%A5%E5%B0%86CPU%E6%83%B3%E8%B1%A1%E4%B8%BA%E4%B8%80%E4%B8%AA%E5%AF%86%E5%B0%81%E7%9A%84%E5%8C%85%EF%BC%8C%E5%9C%A8%E8%BF%99%E4%B8%AA%E5%8C%85%E5%86%85%E6%9C%89%E5%A4%9A%E4%B8%AA%E4%BA%92%E7%9B%B8%E8%BF%9E%E6%8E%A5%E7%9A%84CORES%EF%BC%8C%E6%AF%8F%E4%B8%AACORE%E5%85%B1%E4%BA%AB%E4%B8%80%E4%B8%AA%E4%B8%BB%E5%AD%98%EF%BC%8C%E6%89%80%E6%9C%89%E7%9A%84%E5%A4%84%E7%90%86%E6%A0%B8%E5%BF%83%E9%83%BD%E5%8F%AF%E4%BB%A5%E8%AE%BF%E9%97%AE%E4%B8%BB%E5%AD%98%E3%80%82%20%E5%88%86%E5%B8%83%E5%BC%8F%E5%86%85%E5%AD%98%E7%B3%BB%E7%BB%9F%E6%98%AF%E7%94%B1%E5%A4%9A%E4%B8%AA%E5%A4%84%E7%90%86%E5%99%A8)。
- **矩阵乘法**：数学上的矩阵相乘运算，在并行计算中常被用作典型案例，因为其各元素的计算相互独立，具有良好的并行性[[cnblogs.com]](https://www.cnblogs.com/chihaoyuIsnotHere/p/10553617.html#:~:text=%E6%AD%A3%E5%A6%82%E5%89%8D%E9%9D%A2%E6%89%80%E8%AE%B2%E7%9A%84%EF%BC%8C%E7%9F%A9%E9%98%B5%E7%9B%B8%E4%B9%98%E8%BF%87%E7%A8%8B%E4%B8%AD%EF%BC%8C%E7%BB%93%E6%9E%9C%E7%9F%A9%E9%98%B5C%E4%B8%AD%E7%9A%84%E6%AF%8F%E4%B8%AA%E5%85%83%E7%B4%A0%E9%83%BD%E6%98%AF%E5%8F%AF%E4%BB%A5%E7%8B%AC%E7%AB%8B%E8%AE%A1%E7%AE%97%E7%9A%84%EF%BC%8C%E5%8D%B3%E5%BD%BC%E6%AD%A4%E4%B9%8B%E9%97%B4%E5%B9%B6%E6%97%A0%E4%BE%9D%E8%B5%96%E6%80%A7%E3%80%82%E6%89%80%E4%BB%A5%E5%A6%82%E6%9E%9C%E9%87%87%E7%94%A8%E6%9B%B4%E5%A4%9A%E7%9A%84%E5%A4%84%E7%90%86%E5%99%A8%EF%BC%8C%E5%B0%86%E4%BC%9A%E6%98%BE%E8%91%97%E5%9C%B0%E6%8F%90%E9%AB%98%E7%9F%A9%E9%98%B5%E7%9B%B8%E4%B9%98%E7%9A%84%E8%AE%A1%E7%AE%97%E6%95%88%E7%8E%87%E3%80%82)。
- **加速比**：并行程序的性能提升指标，定义为串行执行时间与并行执行时间的比值[[zhuanlan.zhihu.com]](https://zhuanlan.zhihu.com/p/658581660#:~:text=%E5%8A%A0%E9%80%9F%E6%AF%94%20%E5%8A%A0%E9%80%9F%E6%AF%94%EF%BC%88speedup%EF%BC%89%EF%BC%8C%E6%98%AF%E5%90%8C%E4%B8%80%E4%B8%AA%E4%BB%BB%E5%8A%A1%E5%9C%A8%E5%8D%95%E5%A4%84%E7%90%86%E5%99%A8%E7%B3%BB%E7%BB%9F%E5%92%8C%E5%B9%B6%E8%A1%8C%E5%A4%84%E7%90%86%E5%99%A8%E7%B3%BB%E7%BB%9F%E4%B8%AD%E8%BF%90%E8%A1%8C%E6%B6%88%E8%80%97%E7%9A%84%E6%97%B6%E9%97%B4%E7%9A%84%E6%AF%94%E7%8E%87%EF%BC%8C%E7%94%A8%E6%9D%A5%E8%A1%A1%E9%87%8F%E5%B9%B6%E8%A1%8C%E7%B3%BB%E7%BB%9F%E6%88%96%E7%A8%8B%E5%BA%8F%E5%B9%B6%E8%A1%8C%E5%8C%96%E7%9A%84%E6%80%A7%E8%83%BD%E5%92%8C%E6%95%88%E6%9E%9C%E3%80%82%20Sp=T1/Tp%20Sp%E6%98%AF%E5%8A%A0%E9%80%9F%E6%AF%94%EF%BC%8CT1%E6%98%AF%E5%8D%95%E5%A4%84%E7%90%86%E5%99%A8%E4%B8%8B%E7%9A%84%E8%BF%90%E8%A1%8C%E6%97%B6%E9%97%B4%EF%BC%8CTp%E6%98%AF%E5%9C%A8%E6%9C%89P%E4%B8%AA%E5%A4%84%E7%90%86%E5%99%A8%E2%80%A6)。
- **临界区**：在并行程序中，一次只允许一个线程执行的代码段，用于保护共享资源避免竞态条件。
- **归约操作**：将多个线程的结果合并成一个最终结果的操作（如求和、求最大值等），OpenMP的`reduction`子句可以自动处理这类操作。
- **任务（Task）**：OpenMP中表示一个独立的工作单元，可被动态创建并由线程池中的线程执行，用于支持更灵活的并行模式。
- **Cannon算法**：一种分布式矩阵乘法算法，将进程排列成二维网格，通过循环移位子块数据来实现并行计算，具有存储效率高的优点[[en.wikipedia.org]](https://en.wikipedia.org/wiki/Cannon's_algorithm#:~:text=The%20main%20advantage%20of%20the,number%20of%20processors.)。
- **SUMMA**：可扩展的通用矩阵乘法算法，通过广播矩阵块并累加外积的方式实现分布式矩阵乘法，具有较高的灵活性和负载均衡性[[github.com]](https://github.com/const-sambird/summa#:~:text=This%20repository%20benchmarks%20the%20performance,repository,%20report)。