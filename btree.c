#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYS 3  // 每个节点最多存储的键数量

// 定义 B+ 树节点结构
typedef struct BPlusNode {
    int isLeaf;             // 标志该节点是否为叶子节点
    int keyCount;           // 当前节点的键的数量
    int keys[MAX_KEYS];     // 存储键的数组
    void *children[MAX_KEYS + 1];  // 存储子节点的指针
    struct BPlusNode *next; // 叶子节点通过 next 链接
} BPlusNode;

// 定义 B+ 树结构
typedef struct BPlusTree {
    BPlusNode *root; // 树的根节点
} BPlusTree;

// 创建一个新的 B+ 树节点
BPlusNode* createBPlusNode(int isLeaf) {
    BPlusNode *newNode = (BPlusNode*)malloc(sizeof(BPlusNode));
    newNode->isLeaf = isLeaf;
    newNode->keyCount = 0;
    newNode->next = NULL;
    for (int i = 0; i < MAX_KEYS + 1; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

// 创建一个新的 B+ 树
BPlusTree* createBPlusTree() {
    BPlusTree *tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = createBPlusNode(1); // 默认根节点为叶子节点
    return tree;
}

// 查找节点，返回插入位置
int findInsertPosition(int *keys, int keyCount, int key) {
    int i = 0;
    while (i < keyCount && keys[i] < key) {
        i++;
    }
    return i;
}

// 插入键到叶子节点
void insertIntoLeaf(BPlusNode *leaf, int key, void *value) {
    int i = findInsertPosition(leaf->keys, leaf->keyCount, key);

    // 将后面的键和指针后移
    for (int j = leaf->keyCount; j > i; j--) {
        leaf->keys[j] = leaf->keys[j - 1];
        leaf->children[j] = leaf->children[j - 1];
    }

    // 插入新键
    leaf->keys[i] = key;
    leaf->children[i] = value;  // 假设我们存储值的地址
    leaf->keyCount++;
}

// 分裂叶子节点
BPlusNode* splitLeafNode(BPlusNode *leaf) {
    BPlusNode *newLeaf = createBPlusNode(1);
    int mid = MAX_KEYS / 2;

    // 将一半的键和指针移动到新节点
    for (int i = mid; i < MAX_KEYS; i++) {
        newLeaf->keys[i - mid] = leaf->keys[i];
        newLeaf->children[i - mid] = leaf->children[i];
    }

    // 更新叶子节点的信息
    leaf->keyCount = mid;
    newLeaf->keyCount = MAX_KEYS - mid;

    // 链接新叶子节点
    newLeaf->next = leaf->next;
    leaf->next = newLeaf;

    return newLeaf;
}

// 插入到根节点
void insertIntoRoot(BPlusTree *tree, int key, void *value) {
    BPlusNode *root = tree->root;
    if (root->keyCount < MAX_KEYS) {
        insertIntoLeaf(root, key, value);
    } else {
        // 根节点分裂
        BPlusNode *newRoot = createBPlusNode(0);  // 创建新的非叶子根节点
        BPlusNode *newLeaf = splitLeafNode(root);

        newRoot->keys[0] = newLeaf->keys[0];
        newRoot->children[0] = root;
        newRoot->children[1] = newLeaf;
        newRoot->keyCount = 1;

        tree->root = newRoot;  // 更新根节点
    }
}

// 向 B+ 树插入一个键值对
void insert(BPlusTree *tree, int key, void *value) {
    BPlusNode *root = tree->root;
    if (root->isLeaf) {
        insertIntoLeaf(root, key, value);
    } else {
        insertIntoRoot(tree, key, value);
    }
}

// 打印 B+ 树的内容（用于调试）
void printTree(BPlusNode *node) {
    if (node == NULL) {
        return;
    }

    if (node->isLeaf) {
        printf("叶子节点: ");
        for (int i = 0; i < node->keyCount; i++) {
            printf("%d ", node->keys[i]);
        }
        printf("\n");
    } else {
        printf("非叶子节点: ");
        for (int i = 0; i < node->keyCount; i++) {
            printf("%d ", node->keys[i]);
        }
        printf("\n");
        for (int i = 0; i <= node->keyCount; i++) {
            printTree((BPlusNode*)node->children[i]);
        }
    }
}

// 测试 B+ 树的实现
int main() {
    BPlusTree *tree = createBPlusTree();

    // 插入数据
    insert(tree, 10, "Value 10");
    insert(tree, 20, "Value 20");
    insert(tree, 5, "Value 5");
    insert(tree, 30, "Value 30");
    insert(tree, 25, "Value 25");

    // 打印 B+ 树
    printTree(tree->root);

    return 0;
}
