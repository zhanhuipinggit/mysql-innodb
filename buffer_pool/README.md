## buffer pool
### mysql buffer Pool的实现
- buffer_pool实现简易版buffer pool, day_one_bpoll.c 文件 one 代表最初版，后面迭代会创新新的文件，方便大家归纳学习

### one 版本
- 功能：
  - 简单的buffer Pool
  - 数据结构设计
     - `Buffer Page`：表示数据库中的一页数据，包含数据内容和标志位等。
     - `Buffer Pool`：管理多个 Buffer Page 的缓存池，负责页面的读写和替换。
- 代码解释
  - 数据结构：
    - `buffer_page_t`：每个 buffer_page_t 结构体代表数据库中的一页，包含数据、脏页标志、页ID 和指向下一个页的指针。
    - `buffer_pool_t`：buffer_pool_t 结构体表示缓冲池，包含存储页的数组、缓冲池容量和当前页数等信息。
  - 初始化与加载：
    - `buffer_pool_init`()：初始化缓冲池，设置容量并为页表分配空间。
    - `load_page_from_disk`()：模拟从磁盘加载数据页面。
    - `write_page_to_disk`()：模拟将脏页写回磁盘。
  - 缓冲池操作：
    - `load_page_to_buffer`()：加载指定的页到缓冲池。如果缓冲池已满，会选择一个脏页进行替换。
    - `write_page`()：将数据写入缓冲池中的页面，并标记为脏页。
    - `read_page`()：从缓冲池中读取页面数据。
  - 内存管理：
    - 在 buffer_pool_destroy() 中，销毁缓冲池并释放内存。
