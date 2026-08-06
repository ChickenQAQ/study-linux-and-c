#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "haxi.h"
#define SIZE 20
int hash (datatype key)
{
    return key % SIZE;
}
static void showhashTable(hashTable *ht)
{
    for (int i = 0; i < ht->capacity; i++)
    {
        hashNode *n = ht->table[i];
        if (n != NULL)
        {
            printf("哈希值为:%d:", i);
            while (n != NULL)
            {
                printf(" -> %d", n->key);
                n = n->next;
            }
            printf("\n");
        }
    }
}
int main(void)
{
    // 哈希表的初始化
    hashTable *ht = NULL;
    ht = createhashTable(SIZE);

    // 使用除留余数法/冲突链表的形式，造表
    srand(time(NULL));
    int i;
    for(i=0; i<10; i++)
    {
        inserthashTable(ht, rand()%1000);
        printf("插入键值为:%d\n", rand()%1000);
    }
    showhashTable(ht);

    // 查表
    printf("请输入要查找的键值:");
    int key; scanf("%d", &key);
    findhashTable(ht, key);
    destroyhashTable(ht);
  
    // ....

    return 0;
}