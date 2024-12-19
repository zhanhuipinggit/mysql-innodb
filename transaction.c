#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_LENGTH 100  // 假设最大键的长度为 100
#define MAX_VALUE_LENGTH 100 // 假设最大值的长度为 100

// 数据项结构
typedef struct Data {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} Data;

// 事务日志项结构
typedef struct TransactionLog {
    char key[MAX_KEY_LENGTH];
    char oldValue[MAX_VALUE_LENGTH]; // 存储旧值，以便回滚
    char newValue[MAX_VALUE_LENGTH]; // 存储新值
    struct TransactionLog *next;     // 日志链表
} TransactionLog;

// 事务结构
typedef struct Transaction {
    TransactionLog *log;  // 事务的日志链表
    int isActive;         // 事务是否激活
} Transaction;

// 数据库结构
typedef struct Database {
    Data *dataStore;        // 数据存储（模拟）
    int dataCount;          // 数据项数量
    Transaction *currentTransaction; // 当前事务
} Database;

// 创建数据库
Database* createDatabase() {
    Database *db = (Database*)malloc(sizeof(Database));
    db->dataStore = NULL;
    db->dataCount = 0;
    db->currentTransaction = NULL;
    return db;
}

// 创建事务
Transaction* createTransaction() {
    Transaction *transaction = (Transaction*)malloc(sizeof(Transaction));
    transaction->log = NULL;
    transaction->isActive = 1;
    return transaction;
}

// 查找数据库中的数据项
Data* findData(Database *db, const char *key) {
    for (int i = 0; i < db->dataCount; i++) {
        if (strcmp(db->dataStore[i].key, key) == 0) {
            return &db->dataStore[i];
        }
    }
    return NULL;
}

// 插入数据项
void insertData(Database *db, const char *key, const char *value) {
    db->dataStore = (Data*)realloc(db->dataStore, (db->dataCount + 1) * sizeof(Data));
    strcpy(db->dataStore[db->dataCount].key, key);
    strcpy(db->dataStore[db->dataCount].value, value);
    db->dataCount++;
}

// 添加事务日志
void addTransactionLog(Transaction *transaction, const char *key, const char *oldValue, const char *newValue) {
    TransactionLog *log = (TransactionLog*)malloc(sizeof(TransactionLog));
    strcpy(log->key, key);
    strcpy(log->oldValue, oldValue);
    strcpy(log->newValue, newValue);
    log->next = transaction->log;
    transaction->log = log;
}

// 启动事务
void startTransaction(Database *db) {
    if (db->currentTransaction != NULL && db->currentTransaction->isActive) {
        printf("已有事务未提交，不能启动新事务！\n");
        return;
    }
    db->currentTransaction = createTransaction();
    printf("事务已启动。\n");
}

// 提交事务
void commitTransaction(Database *db) {
    if (db->currentTransaction == NULL || !db->currentTransaction->isActive) {
        printf("没有正在进行的事务。\n");
        return;
    }

    // 清空事务日志
    free(db->currentTransaction->log);
    db->currentTransaction->log = NULL;

    db->currentTransaction->isActive = 0;  // 事务提交
    printf("事务已提交。\n");
}

// 回滚事务
void rollbackTransaction(Database *db) {
    if (db->currentTransaction == NULL || !db->currentTransaction->isActive) {
        printf("没有正在进行的事务。\n");
        return;
    }

    // 遍历事务日志，恢复数据
    TransactionLog *log = db->currentTransaction->log;
    while (log != NULL) {
        Data *data = findData(db, log->key);
        if (data != NULL) {
            // 恢复旧值
            strcpy(data->value, log->oldValue);
        }
        log = log->next;
    }

    // 清空事务日志
    free(db->currentTransaction->log);
    db->currentTransaction->log = NULL;

    db->currentTransaction->isActive = 0;  // 事务回滚
    printf("事务已回滚。\n");
}

// 更新数据项
void updateData(Database *db, const char *key, const char *newValue) {
    if (db->currentTransaction == NULL || !db->currentTransaction->isActive) {
        printf("没有正在进行的事务，无法更新数据。\n");
        return;
    }

    Data *data = findData(db, key);
    if (data == NULL) {
        printf("数据项未找到，插入新数据。\n");
        insertData(db, key, newValue);
        data = findData(db, key);
    }

    // 存储旧值并更新新值
    char oldValue[MAX_VALUE_LENGTH];
    strcpy(oldValue, data->value);
    strcpy(data->value, newValue);

    // 记录事务日志
    addTransactionLog(db->currentTransaction, key, oldValue, newValue);

    printf("数据已更新：%s -> %s\n", key, newValue);
}

// 打印数据库内容
void printDatabase(Database *db) {
    printf("数据库内容：\n");
    for (int i = 0; i < db->dataCount; i++) {
        printf("键：%s, 值：%s\n", db->dataStore[i].key, db->dataStore[i].value);
    }
}

int main() {
    Database *db = createDatabase();

    // 启动事务
    startTransaction(db);

    // 插入数据
    insertData(db, "key1", "value1");
    insertData(db, "key2", "value2");

    // 更新数据
    updateData(db, "key1", "newValue1");

    // 打印数据库内容
    printDatabase(db);

    // 回滚事务
    rollbackTransaction(db);

    // 打印数据库内容（回滚后）
    printDatabase(db);

    // 启动新的事务
    startTransaction(db);

    // 更新数据并提交
    updateData(db, "key2", "newValue2");
    commitTransaction(db);

    // 打印数据库内容（提交后）
    printDatabase(db);

    return 0;
}
