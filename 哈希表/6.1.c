#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define DATATYPE char *
#include "haxi.h"

const char char_pool[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
#define STR_LEN (sizeof(char_pool) - 1)
void randstr(char *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        buf[i] = char_pool[rand() % STR_LEN];
    }
    buf[len] = '\0';
}
int hash (datatype key)
{
    if(key[0]>='a'&&key[0]<='z')
    return key[0] - 'a';
    else if(key[0]>='0'&&key[0]<='9')
    return key[0] - '0';
    else if(key[0]>='A'&&key[0]<='Z')
    return key[0] - 'A';
    else
    return -1;
}
int main(void)
{
    srand(time(NULL));
    hashTable *ht = createhashTable(26);
    if (ht == NULL)
    {
        printf("创建哈希表失败\n");
        return 1;
    }
    char buf[11];
    int len = 10;
    for(int i=0;i<10;i++)
    {
        randstr(buf, len);
        char *key = strdup(buf);  // 每个 key 独立分配内存
        inserthashTable(ht, key);
        printf("插入键值为:%s\n", key);
    }
    showhashTable(ht);
    char s[11]; scanf("%s", s);
    findhashTable(ht, s);
    destroyhashTable(ht);


    return 0;
}