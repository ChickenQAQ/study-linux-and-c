#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct
{
    int capacity; // 容量
    int last;     // 下标
    int *data;    // 顺序表
} seq;
seq *initlist(int n) // n为顺序表大小
{
    seq *list = malloc(sizeof(seq));
    if (list != NULL)
    {
        list->data = malloc(n * sizeof(int));
        if (list->data == NULL)
        {
            free(list);
            return NULL;
        }
        list->capacity = n;
        list->last = -1;
    }

    return list;
}
// 判断顺序表是否已满
bool isfull(seq *list)
{
    return list->last == list->capacity - 1;
}
// 插入数据到头节点
bool insert(seq *list, int data)
{
    if (isfull(list))
    {
        return false;
    }
    for (int i = list->last; i >= 0; i--)
    {
        list->data[i + 1] = list->data[i];
    }
    list->data[0] = data;
    list->last++;
    return true;
}
// 查看顺序表
void showlist(seq *list)
{
    for (int i = 0; i <= list->last; i++)
        printf("%d", list->data[i]);
    printf("\n");
}
// 倒叙输出
void showlist_last(seq *list)
{
    for (int i = list->last; i >= 0; i++)
        printf("%d", list->data[i]);
    printf("\n");
}

// 删除顺序表中的数据
bool delete_data(seq *list, int n)
{
    for (int i = 0; i <= list->last; i++)
    {
        if (list->data[i] == n)
        {
            for (int j = i; j < list->last; j++)
                list->data[j] = list->data[j + 1];
            list->last--;
            return true;
        }
    }
    return false;
}
// 毁灭掉顺序表
void destroy(seq *list)
{
    if (list == NULL)
        return;
    free(list->data);
    free(list);
}
// 冒泡排序

void soft(seq *list)
{
    for (int i = 0; i < list->last; i++)
    {
        int q = 0;
        int n = list->last;
        for (int j = 0; j < n; j++)
        {
            if (list->data[j] > list->data[j + 1])
            {
                int tmp = list->data[j];
                list->data[j] = list->data[j + 1];
                list->data[j + 1] = tmp;
                q = 1;
                n = j;
            }
        }
        if (q == 0)
        {
            printf("原顺序表已有序\n");
            return;
        }
    }
}
int main(int argc, char const *argv[])
{
    seq *list = initlist(10);
    if (list == NULL)
    {
        printf("初始化失败\n");
    }
    else
        printf("初始化成功\n");
    int n;
    for (int i = 5; i > 0; i--)
    {
        scanf("%d", &n);
        insert(list, n);
    }
    showlist(list);
    soft(list);
    showlist(list);
    return 0;
}
