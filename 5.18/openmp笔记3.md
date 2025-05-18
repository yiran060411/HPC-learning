# openmp笔记-3

# OMP指令(二)

## for/do

如果可并行执行的任务数必须在运行时才能确定，且多个任务可通过循环处理，则可采用for/do结构来并行。指令和循环之间不能有其他语句，尾部隐含同步，可用nowait取消。

```c
#pragma omp for clause...
for (i = 0; i < n; i++)
{
...
}
```

该指令的作用是将最近的for/do循环(计数循环)中的有限次数迭代按照一定方式分派给并行域中的所有线程，各个线程同时执行分配到的迭代任务。适用于每次迭代计算量差异不大的情形。

可用子句：

private，firstprivate，lastprivate，reduction，schedule，collapse，linear，ordered，allocate，order(concurrebt)

<aside>
❤️‍🔥

注意：任务之间无依赖；只能用于计数循环；执行过程中不能跳出循环

</aside>

### schedule(kind[, chunk])子句

该子句用于设置循环迭代的分配方式(kind)，可选值有：static，dynamic，guided，auto，runtime。chunk为整数，对于不同的kind的意义不同。

1. **schedule(static, chunk)**
`#pragma omp for schedule(static, chunk)` 
首先将循环迭代划分为多个块，按照线程号顺序，依次将各个块分派给每个线程。分派完成后，再开始执行。适用于每次迭代的任务量相当的情况。
如未指定参数chunk，默认把循环迭代平均分配，块数等于线程数；
如制定了参数chunk，则每个块均包含chunk次迭代(其中一个可以小于chunk)。
2. **schedule(dynamic, chunk)**
动态分配每个工作块：按照chunk值(缺省值为1)将循环划分为若干块，并为每个线程分配一个块，当线程完成一个工作块后，为其分配新的工作块，直至所有块分配完毕。此方案适合各块计算耗时差异较大的情形。
动态方法提高了执行能力和效率，但在处理和分配工作块的时候，产生了更多的计算冗余，每个工作块越大，冗余越少，但也会扩大各县城的工作量的差异。
3. **schedule(guided, chunk)**
动态分配每个工作块：随着计算进行，逐步减少每个块的工作量(通常是将剩余工作量的一半进行分配)，但每个块最少包含 chunk次迭代(最后一个块除外)。如果未指定chunk，默认为1。
例如线程数p=3，迭代次数n=62，chunk=6，则每个线程获得的迭
代次数q：	
第一次分配：q = n / (2p) = 62 / 6 = 10
第二次分配：q = (n-10p) / (2p)= 32 / 6 = 5，q = 6
第三次分配：q = (32-18) / 6 = 14 / 6 = 2, q = 6，6，2
4. **schedule(auto)**
程序自行决定分派方式。
5. **schedule(runtime)**
前面的几种方案在编译源代码时就固定了，如果需要在运行时修改分配方式，则可指定为RUNTIME方式，并由环境变量OMP_SCHEDULE决定。

大多数情况(尤其是每次迭代任务相当时)无需使用schedule子句，默认就行。

### callapse(n)子句

默认情况下，omp for/do仅将最近的循环并行化，collapse可将n层循环合并为一个大循环进行并行。遵守以下限制条件：

1. n层循环之间不能有其他语句	
2. 循环域规则

### linear子句

表明变量为firstprivate属性，且其值与关联的循环的迭代次序线性关。

## ordered

如果omp for/do结构中某部分代码需要按迭代顺序执行(其余代码行)，可使用ordered结构。此时需在omp for/do结构开始处指定ordered子句。

```c
#pragma omp ordered clause ...
{	
...代码块
}
```

```fortran
!$omp ordered clause ...	
	...代码块	
!$omp end ordered
```

可用子句：threads(默认) 或simd

## task

omp for/do结构可以很方便地将循环并行化，但需要在并行之前确切知道循环次数，才能划分工作块。也可以使用taskloop结构并行。
当循环次数未知(或无穷)时，如while循环、递归、链表、数据实时处理等，OMP 3.0提供了task结构来显式定义任务，并交由某个线程执行。

```c
#pragma omp task clause ...		
{	
...代码块
}	
```

```fortran
!$omp task clause ...	
...代码块
!$omp end task
```

### 可用子句

if、default、final、untied、mergeable、private、
firstprivate、shared、in_reduction、depend、priority、 allocate、affinity、detach
untied：消除任务和线程之间的绑定关系，任务暂停后，任何线程均可恢复执行该任务。

<aside>
🐳

task结构一般位于single结构中，表示仅由单一线程去生成显式任务，任务生成之后，可由任意线程去执行(untied子句)。

</aside>

## taskloop

计数循环中，各迭代的计算量差异较大时，可使用taskloop结构来行。

```c
#pragma omp taskloop clause ...	!$omp taskloop clause ...	
{
...代码块
}
```

```fortran
!$omp taskloop clause ...	
...代码块
!$omp end taskloop
```

可用子句：if、default、final、shared、private、firstprivate、lastprivate、reduction、in_reduction、grainsize、num_tasks 、collapse、priority、untied, mergeable, nogroup、allocate

**grainsize：**

用于限定每个task获得的迭代次数n

$min(grainsize, nlts)<= n < 2*grainsize$
**num_tasks：**

限定task的数量$min(numtasks,nlts)$，与grainsize互斥(不能同时出现)
**priority：**

设置优先级(默认0)

<aside>
🍃

nlts表示循环迭代的次数，min取最小值

</aside>

## loop

如果循环迭代之间不存在数据依赖(互不相关)，可使用omp loop结构，其功效类似于workshare。

```c
#pragma omp loop clause ...
{
...for循环
}
```

```fortran
!$omp loop clause ...
...do循环
!$omp end loop
```

### 子句

bind、collapse、order(concurrent)、private、 lastprivate、reduction

## 复合结构

如果并行域中仅包含一个结构，则可将此结构与并行域一起写为复合结构，它们的子句均可用在复合结构上。常用有：

1. !$omp parallel sections  /  !$omp end parallel sections
2. !$omp parallel workshare  /  !$omp end parallel workshare
3. !$omp parallel do / loop  /  !$omp end parallel do / loop
4. !$omp parallel master taskloop  /  !$omp end parallel master taskloop

<aside>
🐻

这里只给出Fortran的写法，C语言同样适用。

</aside>

## 计时函数

1. **omp_get_wtime函数**
返回以秒为单位的绝对时间，调用两次就可计算出它们之间的代码的执行时间。
2. **cpu_time子程序**
fortran标准函数，返回以秒为单位的近似的处理机时间(所有线程的任务量总和除以时钟频率)，该值仅反映任务量大小。对于串行代码，其值近似等于执行时间;对于并行代码，其值大于程序执行时间。
3. **clock**
与cpu_time类似，返回处理机时钟次数，结果除以CLOCKS_PER_SE后转为以秒为单位的时间。

**加速比：**串行时间除以并行时间，其值接近(小于)线程数

**效率：**加速比除以线程数，不大于1