# openmp笔记

## 构造并行域

### 并行域

OMP以Fork-Join模式并行并执行特定部分代码，这些特定代码代码所组成的动态范围(dynamic extent)称为并行域。以下指令对用于构造一个并行域，包含于其中的代码或任务可被并行执行:

```c
#pragma omp parallel clause ...
{
	...
}
```

```fortran
!$omp parallel clause1 clause2 ...
		...
!$omp end parallel
```

<aside>
📖 1.C大小写敏感，Fortran不区分大小写(预处理区分)
2.并行域中的代码不一定并行执行

</aside>

### 执行模式

- 启动程序，主线程(线程0)执行代码块1
- 遇到`!$omp parallel` 后，主线程分裂(fork)为多个线程，每个线程均尝试执行代码块2
- 各线程遇到`!$omp end parallel` 后，合并(join)为一个线程(主线程)，并由主线程执行代码块3
- 主线程结束程序

<aside>
😒 文本范围(lexical extent):parallel指令对之间的语句
动态范围(dynamic extent):并行域中所有被执行的语句

</aside>

### 设置线程数

一个完整的OMP程序，其中可以包含若干个串行区域和若干个并行区域，各个并行区域内的线程数可以不同。线程数可有环境变量(或内部控制变量ICV，可认为是默认值)、OMP函数(子程序)、并行域的字句等确定。

- 环境变量`OMP_NUM_THREADS`缺省(默认)线程数一般等于CPU核数
- 子程序`omp_set_num_threads` 影响后续所有并行域的线程数
- 子句`num_threads`设置当前并行域的线程数

<aside>
💕 优先级:子句 > 子程序 > 环境变量

</aside>

### 常用函数(子程序)

`omp_set_max_activate_levels`:最大嵌套并行层数

`omp_get_num_procs`:获取CPU核心数

`omp_set_num_threads`:设置后续所有并行域的线程数

`omp_get_num_threads`:获取函数调用处线程数

`omp_get_threads_num`:获取当前线程的线程号

<aside>
🥯 一般set为子程序(无返回值)，get为函数

</aside>

### 线程数多少

OMP的线程数可为任意正整数(实际受限，可由`omp_get_thread_linit`函数查询)，但线程数不宜超过CPU物理核心数(CPU的超线程对计算密集型任务无益)，主要原因:

- 开辟、释放线程比较耗时
- 单个核心处理多个线程时会降低效率(线程切换)
- 线程数过多造成内存和堆栈负担

### 嵌套并行域

在并行域中再次开辟新的并行域，称嵌套并行域。子程序`omp_set_max_activate_levels`用于设置执行时的最大嵌套并行层数n(默认n=1)。当设置n=0时，并行域失效，代码串行执行；n>1时，可嵌套执行。如果代码并行域的嵌套层数为m，则实际嵌套并行 层数为min(m,n)。

### 子句

OMP指令之后常常可以添加一个或多个子句(clause)，用以进行补充说明。子句用于控制具体行为、数据属性、或二者兼之。并行域处的子句有:

- 并行域开头(`#pragma omp parallel` / `!$omp parallel`)可用子句有: `if,  num_threads,  default,  private,  firstprivate,  shared,  reduction,  copyin,  proc_bind,  allocate`
- 并行域结尾(`!$omp end parallel`)无可用子句，隐含线程同步

OMP的数据属性可归结为三类: shared,  private,  threadprivate。进入并行域时，如果变量具有私有属性(peivate,  firstprivate,  lastprivate,  reduction,  linear)，则会额外开辟nT份(线程数)临时空间来存储数据，并在退出该结构时销毁临时数据。

一个示意图来解释OpenMP中的数据属性和内存分配:

```
主内存空间                       线程私有临时空间
+-------------+            Thread 0: +-------------+
|   shared    |            private   |   copy 1    |
|  variables  |            data      |             |
|             |                      +-------------+
+-------------+            Thread 1: +-------------+
                          private   |   copy 2    |
                          data      |             |
                                   +-------------+
                          Thread 2: +-------------+
                          private   |   copy 3    |
                          data      |             |
                                   +-------------+
                             ...

注: 
- shared变量: 所有线程共享同一份内存
- private变量: 每个线程获得独立的临时副本
- 退出并行域时，临时空间被释放
```

1. if(b)
根据参数b的值判断并行域是否有效，进而决定构建并行或串行代码。
    
    <aside>
    🥘  b = 0/false → 串行
     b = 非零/true → 并行
    
    </aside>
    
2. num_threads(nT)
参数nT为整型变量或表达式(正整数)，设置并行域内的线程数。
3. default(private | firstprivate | shared | none)
设置并行域内的变量的默认属性，即如果没有显式给定变量的属性，则其属性为默认属性。参数为上述四种(私有、承前私有、共享、无)中的其中一种(C语言只可选shared | none)。
    
    <aside>
    🦇 C语言中，并行区域内变量也可有private | firstprivate属性，只是default子句中仅有两种可选。C可在并行域定义局部变量(私有)
    
    </aside>
    
4. private(a, b, c)
变量a, b, c等为私有变量，每个线程都生成变量的私有备份，同一变量名在各个线程中可以有不同值。初始值未知。
5. firstprivate(a, b, c…)
变量a, b, c等为私有变量，每个线程都生成变量的私有备份，同一变量名在各个线程中可以有不同值。初始值为主线程的值。
6. shared(a, b, c…)
变量a, b, c等为共享变量，所有线程共享同一片内存(一荣俱荣)。初始值为主线程的值
7. reduction(operator : list)
规约：退出并行域时，按照约定的操作将同名变量的多个值处理为一个值。列表中的变量自动含有private属性。可用的操作或函数：
C语言
操作符：+，-，*，&，|，^，&&，||
Fortran
操作符：+，*，-，.AND.，.OR.，.EQV.，.NEQV.
内置函数：MAX，MIN，IAND，IOR，IEOR
    
    ![微信图片编辑_20250516202944.jpg](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87%E7%BC%96%E8%BE%91_20250516202944.jpg)
    
8. copyin
仅用于线程私有(threadprivate)变量，进入并行域时将主线程的值赋值给其他线程。
9. proc_bind
设置线程与CPU核心之间的绑定关系，极少用。
10. allocate
指针或动态数组可在并行域分配，将增加代码的复杂度，不建议使用，可用子过程规避。