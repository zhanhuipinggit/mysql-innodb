//
// Created by zhanJames on 2024/12/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义内存池的大小
#define POOL_SIZE 1024 * 1024  // 1MB 的内存池

// 内存块的结构体
typedef struct Block {
    size_t size;        // 当前内存块的大小
    struct Block *next; // 指向下一个空闲内存块
} Block;

// 定义内存池的结构体
typedef struct MemoryPool {
    char *start;   // 内存池的起始位置
    Block *freeList; // 空闲内存块的链表
} MemoryPool;

// 初始化内存池
MemoryPool* createMemoryPool(size_t poolSize) {
    MemoryPool *pool = malloc(sizeof(MemoryPool));
    if (pool == NULL) {
        printf("内存池创建失败！\n");
        return NULL;
    }

    // 为内存池分配空间
    pool->start = malloc(poolSize);
    if (pool->start == NULL) {
        printf("内存池分配失败！\n");
        free(pool);
        return NULL;
    }

    // 初始化空闲链表
    pool->freeList = (Block*) pool->start;
    pool->freeList->size = poolSize - sizeof(Block);  // 除去 Block 结构的大小
    pool->freeList->next = NULL;

    return pool;
}

// 从内存池中分配内存
void* poolMalloc(MemoryPool *pool, size_t size) {
    Block *prev = NULL;
    Block *curr = pool->freeList;

    // 找到足够大的空闲块
    while (curr != NULL) {
        if (curr->size >= size) {
            // 分配内存块
            void *allocatedMemory = (void *) (curr + 1);  // 内存块的实际数据区域

            // 剩余空间，如果剩余空间足够大，则拆分
            if (curr->size > size + sizeof(Block)) {
                Block *nextBlock = (Block *) ((char *) curr + sizeof(Block) + size);
                nextBlock->size = curr->size - size - sizeof(Block);
                nextBlock->next = curr->next;
                curr->size = size;
                curr->next = nextBlock;
            } else {
                // 如果没有足够的空间进行拆分，直接移除当前空闲块
                if (prev == NULL) {
                    pool->freeList = curr->next;
                } else {
                    prev->next = curr->next;
                }
            }

            return allocatedMemory;
        }

        prev = curr;
        curr = curr->next;
    }

    // 如果没有找到合适的块，返回 NULL
    printf("内存池不足，无法分配 %zu 字节内存！\n", size);
    return NULL;
}

// 释放内存块
void poolFree(MemoryPool *pool, void *ptr) {
    Block *block = (Block *) ptr - 1;  // 返回指向 Block 的指针

    // 将释放的内存块放回空闲链表
    block->next = pool->freeList;
    pool->freeList = block;
}

// 销毁内存池
void destroyMemoryPool(MemoryPool *pool) {
    free(pool->start);
    free(pool);
}

// 用于测试的函数
void testMemoryPool() {
    // 创建一个 1MB 大小的内存池
    MemoryPool *pool = createMemoryPool(POOL_SIZE);
    if (pool == NULL) {
        return;
    }

    // 分配 256 字节
    void *mem1 = poolMalloc(pool, 256);
    printf("分配了 256 字节内存\n");

    // 分配 512 字节
    void *mem2 = poolMalloc(pool, 512);
    printf("分配了 512 字节内存\n");

    // 分配 128 字节
    void *mem3 = poolMalloc(pool, 128);
    printf("分配了 128 字节内存\n");

    // 释放 256 字节的内存
    poolFree(pool, mem1);
    printf("释放了 256 字节内存\n");

    // 再次分配 256 字节
    void *mem4 = poolMalloc(pool, 256);
    printf("再次分配了 256 字节内存\n");

    // 销毁内存池
    destroyMemoryPool(pool);
    printf("销毁内存池\n");
}

// 主函数
int main() {
    // 测试内存池分配
    testMemoryPool();
    return 0;
}
