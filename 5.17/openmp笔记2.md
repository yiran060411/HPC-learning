# openmp笔记-2

# OMP指令(一)

在之前的例子中，并行域(parallel结构)中的语句会被该并行域中的所有线程执行，即相同的代码被多次执行(各线程的执行结果可以不同)，未达到并行计算的目的。为使不同线程执行不同的任务(或同一任务的不同部分)，需要在并行域中使用其他OMP指令(结构)将任务分派给各个线程。

根据实际情况，任务分派可采用sections、workshare、do、task、taskloop等指令。

## sections

sections结构将代码分割成多个独立代码块(sections)，每块代码仅被一个线程执行一次。

```c
#pragma omp sections clause..
{
#pragma omp section
...代码块1
#pragma omp section
...代码块2
}
```

```fortran
!$omp sections clause...
!$omp section
...代码块1
!$omp sectio
...代码块2
!$omp end sections end_clause
```

### **特点**

1. 该结构适用于处理多个完全不同的任务(一边…一边…)
2. section的数量是恒定的(写代码时就确定了)，程序运行过程中不会增减(局限性)
3. 每个代码块(section)组成一个单独的任务，仅被一个线程执行
4. 一个线程可能执行0，1，n个代码块(由线程和代码块数量决定)
5. 紧挨sections的section可以把省略
6. 结构末尾处隐含有线程同步(可用nowait取消同步)

### **子句**

private，firstprivate，lastprivate，reduction，allocate，nowait

lastprivate(a，b，c…)：变量a，b，c等为私有变量，初始值未知，退出结构时，源变量的值为最后一次更新的结果。

nowait：置于结构开头(C)或结尾(Fortran)处，用于取消隐式线程同步，即线程到达该结构的结尾处后无需等待“集合”，继续执行后续任务。

## workshare

该结构仅适用于fortran语言，将数组相关的整体操作划分为多个工作单元，分派给各个线程并行执行。

```fortran
!$omp workshare
...代码块
!$omp end workshare[nowait]
```

其内操作包括：数值整体赋值、forall和where语句

内置函数：MATMUL，DOT_PRODUCT，SUM，PRODUCT，MAXVAL，MINVAL，COUNT，ANY，ALL，SPREAD，PACK，NPACK，PESHAPE，TRANSPOSE，EOSHIFT，MINLOC，MAXLOC

## single/master

如果并行域中某部分代码仅需执行一次，例如输出进度提示信息、读取模型参数等，有以下几种方案：

- 构建“并行 — 串行 — 并行”模式代码。并行域的构建和销毁较为耗时，此方案的效率较低
- 并行域中构建只有一个代码块的sections结构，不能广播数据(难以将变量在某一线程中的值传递给其他线程)
- 并行域中使用判断语句，仅让一个线程(一般是主线程)参与运算，不能广播数据
- 使用single或master结构。一般，single多用于读取并广播数据，master多用于输出提示

### **single结构**

仅被并行域中的一个线程(任意)执行，末尾处含有隐式线程同步。

```c
#pragma omp single clause
{
...代码块
}
```

```fortran
!$omp single clause...
...代码块
!$omp end single end_clause
```

clause：private，firstprivate，allocate

end_clause：copyprivate或者nowait

<aside>
🐘

copyprivate子句写在结构开头(C)或结尾(Fortran)处，作用是将一个私有变量的值传递给其他线程(广播)

</aside>

### **master结构**

仅被并行域中的主线程执行，末尾处没有线程同步，无任何子句。主线程执行代码块的同时，其余线程执行后续任务，一般用于输出提示。

```c
#pragma omp master
{
...代码块
}
```

```fortran
!$omp master
...代码块
!$omp end master
```

## atomic/critical

如果并行域中某部分代码需要被所有线程执行，但不能被多个线程同时执行，例如更新共享变量、读取数据等，可使用atomic或critical结构。

### atomic结构

确保同时只有一个线程修改变量(一般为共享变量)，应用于简单的赋值语句：

```c
#pragma omp atomic [atomic-clause]
{
...代码块
}
```

```fortran
!$omp atomic [atomic-clause]
...代码块
!$omp end atomic
```

atomic-clause：read，write，update，capture

read：仅读取变量x的值，v = x

write：覆盖原有值，x = v

update：对原有值进行更新，x += 1

capture：更新原有值，并传递给其他变量，v = x++

<aside>
📯

- v为私有变量，x为共享变量
- atomic-clause可以不写，默认为update
</aside>

### critical结构

atomic主要用于控制共享变量的更新，应用场景比较简单。对于更加复杂的代码或场景，可用critical结构。临界区里面可以是任意合法代码。

```c
#pragma omp critical (name) -> 临界区的名字，可以不写，即为无名临界区
{
...代码块
}
```

```fortran
!$omp critical (name)
...代码块
!$omp end critical (name)
```

atomic结构的效率更高，critical结构可以实现atomic结构所有功能

**临界区的特点**

1. 所有同名的多个临界区被看作同一个临界区
2. 所有未命名的临界区看作同一个临界区
3. 同一时刻，临界区A、B、C…（为多个不同名字的临界区，其中一个可以为无名临界区）里面分别最多只有一个线程
4. 当某个线程到达临界区A入口时，它会在此等待，检索所有同名临界区里是否有其他线程执行；等待前一个线程执行完毕后再进入临界区。末尾处没有线程同步。

## flush

flush用于强制刷新变量，使缓存与内存一致。

```c
#pragma omp flush (a,b...)
```

```fortran
!$omp flush (a,b...)
```

for example:

线程A对一个共享数组进行操作时，可能需要使用到受线程B所影响那一部分的信息。线程B对相关信息进行操作时使用了缓存，即操作结果在缓存中，并未更新内存中的信息。这种情况下，必须保证要在A读取数组之前执行完内存更新，可采用flush指令。如果指令后不跟变量列表，则刷新所有变量。

## barrier

barrier指令在指定位置插入一个阻塞（同步点），所有线程到达此处后，再继续执行后续语句。

```c
#pragma omp barrier
```

```fortran
!$omp barrier
```

下列结构的结尾处隐含有同步：

parallel、sections、single、workshare

同步点处隐含有flush

barrier限制条件：

1. 不能用于sections、single、master、atomic、critical等结构中
2. 其他可引起死锁的情形，如：

```fortran
if (omp_get_thread_num == 0)
	  !$OMP BARRIER
end if
```