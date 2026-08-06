#ifndef HAXI_H
#define HAXI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DATATYPE
#define DATATYPE int
#endif
typedef DATATYPE datatype;

typedef struct hashNode
{
    datatype key;
    struct hashNode *next;
} hashNode;

typedef struct
{
    hashNode **table;  // 哈希桶数组，每个桶是一个链表头指针
    int capacity;
    int size;
} hashTable;

extern int hash(datatype key);

static hashTable *createhashTable(int capacity)
{
    hashTable *ht = (hashTable *)malloc(sizeof(hashTable));
    if (ht == NULL) return NULL;
    ht->table = (hashNode **)calloc(capacity, sizeof(hashNode *));
    if (ht->table == NULL)
    {
        free(ht);
        return NULL;
    }
    ht->capacity = capacity;
    ht->size = 0;
    return ht;
}

static void inserthashTable(hashTable *ht, datatype key)
{
    int index = hash(key);
    if (index < 0 || index >= ht->capacity)
    {
        printf("哈希值无效,无法插入\n");
        return;
    }
    hashNode *n = (hashNode *)malloc(sizeof(hashNode));
    if (n == NULL) return;
    n->key = key;
    n->next = ht->table[index];  // 头插法
    ht->table[index] = n;
    ht->size++;
}

static void findhashTable(hashTable *ht, datatype key)
{
    int index = hash(key);
    int count = 0;
    if (index < 0 || index >= ht->capacity)
    {
        printf("哈希值无效,无法查找\n");
        return;
    }
    hashNode *n = ht->table[index];
    while (n != NULL)
    {
        if (n->key == key)
        {
            count++;
            printf("找到键值为:%d,查找次数为:%d\n", n->key, count);
            return;
        }
        n = n->next;
        count++;
    }
    printf("未找到键值为:%d,查找次数为:%d\n", key, count);
}


static void destroyhashTable(hashTable *ht)
{
    if (ht == NULL) return;
    for (int i = 0; i < ht->capacity; i++)
    {
        hashNode *n = ht->table[i];
        while (n != NULL)
        {
            hashNode *tmp = n;
            n = n->next;
            free(tmp);
        }
    }
    free(ht->table);
    free(ht);
}
#endif
