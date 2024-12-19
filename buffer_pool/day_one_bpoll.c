#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// 假设数据库页的大小为 4KB
#define PAGE_SIZE 4096
#define NUM_PAGES 10

// 缓存页的结构体
typedef struct buffer_page {
    int page_id;             // 页ID
    char data[PAGE_SIZE];    // 页数据
    int is_dirty;            // 是否脏页
    struct buffer_page* next;  // 指向下一个页的指针（链表形式）
} buffer_page_t;

// 缓存池的结构体
typedef struct buffer_pool {
    buffer_page_t* pages[NUM_PAGES];  // 页表，存储所有缓冲页
    int capacity;                      // 缓冲池容量
    int current_size;                  // 当前缓冲池中的页数
    pthread_mutex_t lock;              // 线程锁，保护缓冲池的并发访问
} buffer_pool_t;

// 全局缓冲池
buffer_pool_t buffer_pool;

// 初始化缓冲池
void buffer_pool_init(int capacity) {
    buffer_pool.capacity = capacity;
    buffer_pool.current_size = 0;
    pthread_mutex_init(&buffer_pool.lock, NULL);

    for (int i = 0; i < NUM_PAGES; i++) {
        buffer_pool.pages[i] = NULL;
    }
}

// 查找页，如果页存在则返回该页指针，否则返回 NULL
buffer_page_t* find_page(int page_id) {
    for (int i = 0; i < NUM_PAGES; i++) {
        if (buffer_pool.pages[i] != NULL && buffer_pool.pages[i]->page_id == page_id) {
            return buffer_pool.pages[i];
        }
    }
    return NULL;
}

// 从磁盘加载页面（模拟）
buffer_page_t* load_page_from_disk(int page_id) {
    // 模拟从磁盘加载数据
    buffer_page_t* page = (buffer_page_t*)malloc(sizeof(buffer_page_t));
    page->page_id = page_id;
    memset(page->data, 0, PAGE_SIZE);  // 初始化数据
    page->is_dirty = 0;
    page->next = NULL;
    return page;
}

// 将页面写回磁盘（模拟）
void write_page_to_disk(buffer_page_t* page) {
    if (page->is_dirty) {
        printf("Writing page %d to disk...\n", page->page_id);
        // 实际写入操作（这里简化为打印信息）
        page->is_dirty = 0;
    }
}

// 将页加载到缓冲池
buffer_page_t* load_page_to_buffer(int page_id) {
    buffer_page_t* page = find_page(page_id);
    if (page != NULL) {
        return page;  // 如果页已经在缓冲池中，直接返回
    }

    // 如果缓冲池已满，选择一个页进行替换
    if (buffer_pool.current_size >= buffer_pool.capacity) {
        // 选择一个脏页写回磁盘（这里选择第一个脏页进行替换）
        for (int i = 0; i < NUM_PAGES; i++) {
            if (buffer_pool.pages[i] != NULL && buffer_pool.pages[i]->is_dirty) {
                write_page_to_disk(buffer_pool.pages[i]);
                free(buffer_pool.pages[i]);
                buffer_pool.pages[i] = NULL;
                buffer_pool.current_size--;
                break;
            }
        }
    }

    // 从磁盘加载页面并添加到缓冲池
    page = load_page_from_disk(page_id);
    for (int i = 0; i < NUM_PAGES; i++) {
        if (buffer_pool.pages[i] == NULL) {
            buffer_pool.pages[i] = page;
            buffer_pool.current_size++;
            return page;
        }
    }
    return NULL;
}

// 写页面到缓冲池，并标记为脏页
void write_page(int page_id, const char* data) {
    buffer_page_t* page = load_page_to_buffer(page_id);
    memcpy(page->data, data, PAGE_SIZE);  // 写入数据
    page->is_dirty = 1;  // 标记为脏页
}

// 读取缓冲池中的页面数据
void read_page(int page_id, char* data) {
    buffer_page_t* page = load_page_to_buffer(page_id);
    memcpy(data, page->data, PAGE_SIZE);  // 复制数据到用户缓冲区
}

// 销毁缓冲池，清理内存
void buffer_pool_destroy() {
    for (int i = 0; i < NUM_PAGES; i++) {
        if (buffer_pool.pages[i] != NULL) {
            write_page_to_disk(buffer_pool.pages[i]);
            free(buffer_pool.pages[i]);
        }
    }
    pthread_mutex_destroy(&buffer_pool.lock);
}

int main() {
    buffer_pool_init(3);  // 初始化缓冲池，最大缓存3页

    // 写数据到页面
    char data[PAGE_SIZE] = "This is page data.";
    write_page(1, data);
    write_page(2, data);
    write_page(3, data);

    // 读取页面数据
    char read_data[PAGE_SIZE];
    read_page(2, read_data);
    printf("Read data from page 2: %s\n", read_data);

    // 销毁缓冲池
    buffer_pool_destroy();
    return 0;
}
