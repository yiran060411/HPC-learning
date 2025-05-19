# MPI笔记-1

# MPI

## MPI概念

**消息传递接口(Message Passing Interface, MPI)**是一个跨语言的通讯协议，其目标是实现具有高性能和可移植性的大规模并行程序。MPI通过在每个节点(机器)启用一个或多个进程来实现并行计算，每个进程还可以含有多个线程，故有时也将MPI和OpenMP结合使用。

## 执行MPI程序

mpi 程序代码中不能指定进程总数(但可获取)，而是在运行该程序时通过命令行参数给定。mpi程序编译完成后，可单独执行，此时仅可在本地启用1个进程。若想在一个或多个节点启用多进程，需使用专门的程序 mpiexec.exe 来启动 mpi 程序:
                                               **mpiexec        参数        MPI程序名**
常用参数:
**-n**                        进程数，缺省为cpu核数
**-hostfile**             后跟文件名，文件中存储节点名，每行一个
**-H,-host**             后跟节点名，以逗号分隔
**-pernode**           每个节点启用一个进程

**MPI** 程序启用多个进程实现并行计算，每个进程拥有独立的内存空间，进程间以消息进行信息交换(MPI3.0引入了共享内存机制，同一节点上的多个进程之间可共享信息，使用比较繁琐)。编写 MPI 程序重心在于**处理通信**。

### 通信

通信函数被调用之后，需要-一定时间来完成通信操作，在此期间：

1. 根据调用进程的反应可将相关函数分为**阻塞通信**和**非阻塞通信：**
前者需等待指定操作的实际完成，或至少所涉及的数据已被安全备份后才返回；
后者不关心相关操作是否完成，调用后即刻返回。
2. 根据存储被发送数据的变量的可用状态，可分为**缓存通信**和**非缓存通信：**
前者首先将数据拷贝至缓冲区，然后对缓冲区数据执行通信操作，缓冲区大小可由系统给定，或通过MPI Buffer attach 和MPI Buffer detach来申请和释放缓冲区；
后者直接对变量执行通信操作。

### 四种通信模式

- **标准通信(standard mode)**：MPI 自行决定是否缓存数据；
- **缓存通信(buffered mode)**：将发送消息拷贝至缓冲区后立即返回，消息的发送由 MPI 系统在后台进行；
- **同步通信(synchronous mode)**：同步发送时，接收方开始接收消息后才可正确返回；
- **就绪通信(ready mode)**：就绪发送时，必须确保接收方已经就绪(正等待接收该消息)，否则该调用将产生一个错误。

### 通信函数举例

---

通信功能
Standard Send
Synchronous Send
Buffered Send
Ready Send
Receive
Completion Check

阻塞
MPI_Send
MPI Ssend
MPI Bsend
MPI Rsend
MPI Recv
MPI Wait

非阻塞
MPI Isend
MPI Issend
MPI Ibsend
MPI Irsend
MPI Irecv
MPI Test

---

### 数据通信方式及常用通信函数

**一对一**
MPI Send
MPI Bcast
MPI Reduce

**一对多**
MPI Alltoall
MPI Recv
MPI Scatter

**多对多**
MPI Scatterv
MPI Gatherv
MPI Alltoallw

**多对一**
MPI Gather
MPI Alltoallv
MPI Sendrecv

<aside>
✅

(MPI_Allreduce, MPI_Allgather, MPI_Allgatherv)

</aside>

## MPI并行模式

### 对等模式

所有进程均为工作进程，共同执行任务(通常将任务平均分配)，适用于每个任务计算量相当的情形(或各进程执行不同任务)； 

### 主从模式

或称管理模式，首先将总任务分为多个任务片段，每个片段包含1个或多个任务，然后选择其中一个进程(一般是主进程)作为管理者为其他进程派送(审批)任务，每次派送一个任务片段。
程序执行时至少需要2个进程，适用于每个任务计算量差异较大的情形。