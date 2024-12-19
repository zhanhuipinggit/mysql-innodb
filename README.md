# mysql-innodb
## 为了更好理解mysql-Innodb引擎这里实现了几个简单的小例子
### btree.c
  - 代码解析
    - B+ 树节点结构（BPlusNode）：

      - 每个节点包含一个键值数组（keys），以及指向子节点的指针数组（children）。
      如果是叶子节点，next 指针用来链接下一个叶子节点，形成链表。
      keyCount 表示当前节点中键的数量，isLeaf 标识当前节点是否为叶子节点。
    - B+ 树的插入操作：
      - insert 函数是 B+ 树的插入接口，首先检查根节点是否为叶子节点，然后根据情况调用不同的插入函数。
      - insertIntoLeaf 函数负责将数据插入到叶子节点。
      - 如果叶子节点已满（keyCount >= MAX_KEYS），则调用 splitLeafNode 函数将节点分裂，形成新的叶子节点，并将中间值上升到父节点。
      - 如果根节点分裂了，就需要创建新的根节点。
    - 打印树结构： 
      - printTree 函数用于打印 B+ 树的内容。它递归遍历每个节点，并根据节点的类型（叶子节点或非叶子节点）进行不同的处理。

    - 这个简单的 B+ 树实现，利用字典（hashmap）来模拟存储数据，通过分裂节点来保证树的平衡性。虽然代码中没有实现实际的磁盘存储操作，但是通过 children 指针来模拟了树形结构。这个示例展示了 B+ 树的核心操作，如插入和分裂，便于理解 B+ 树的基本工作原理。

    - 在实际的数据库或文件系统中，B+ 树被广泛用于索引管理，其高效的查找和插入操作使其非常适合用于处理大量的数据。
--- 
### memory.c
  - 代码解析
    - 内存池结构：
      - MemoryPool 结构体包含了内存池的起始地址 (start) 和空闲链表 (freeList)，空闲链表用于跟踪所有未被分配的内存块。
      - Block 结构体是每个内存块的结构，包含了内存块的大小和指向下一个空闲块的指针。
    - 内存池初始化：
      - createMemoryPool 函数用于创建内存池，并初始化空闲链表。空闲链表最初只有一个大内存块，它的大小是内存池大小减去 Block 结构的大小。
    - 内存分配：
      - poolMalloc 函数从空闲链表中查找第一个足够大的内存块。如果找到合适的块，就将其从空闲链表中移除，并根据需要拆分内存块。
      如果没有找到合适的块，函数返回 NULL，表示内存池不足。
    - 内存释放：
      - poolFree 函数将释放的内存块插回空闲链表，以便后续可以复用。
    - 内存池销毁：
      - destroyMemoryPool 函数释放内存池的内存。
    - 测试函数：
      - testMemoryPool 函数演示了如何创建内存池、分配内存、释放内存和销毁内存池的过程。
  - 总结
    通过这个实现，您可以看到如何利用内存池和空闲链表来管理内存。该内存管理器模拟了一个简单的内存分配系统，支持内存块的分配、释放和复用。您可以进一步扩展这个内存管理器，加入更多的功能，例如内存碎片合并、内存池大小调整等。
---
### transaction.c
  - 代码解析
    - 数据库和事务结构：

      - Database：代表一个简单的数据库，包含了数据项数组（dataStore）和当前活动事务（currentTransaction）。
      - Transaction：代表一个事务，包含一个日志链表（log），记录了事务中的操作。
    - 事务操作：
      - startTransaction：开始一个新的事务，若已有未提交的事务，不能启动新事务。
      - commitTransaction：提交当前事务，所有变更都会被保存。
      - rollbackTransaction：回滚当前事务，撤销所有变更，恢复到事务开始前的状态。
    - 数据操作：
      - insertData：向数据库中插入新的数据项。
      - updateData：更新数据项的值，在更新之前，记录旧值到事务日志中，以便回滚时恢复旧值。
    - 事务日志：
      - 事务日志项（TransactionLog）包含键（key），旧值（oldValue），和新值（newValue），用于在事务回滚时恢复数据。
    - 测试：
      - 在 main 函数中，演示了如何启动事务、执行更新操作、回滚事务、提交事务等基本操作，并打印出数据库的内容，帮助验证事务机制的实现。

## buffer pool
### mysql buffer Pool的实现
- buffer_pool实现简易版buffer pool, day_one_bpoll.c 文件 one 代表最初版，后面迭代会创新新的文件，方便大家归纳学习
