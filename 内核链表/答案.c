#include "kernel_list.h"
#include <stdlib.h>
#include <stdio.h>

// 设计包含具体数据的大结构体
typedef struct node
{
    int data;
    struct list_head list; // 内核链表标准结构体
} listnode, *linklist;

// 所有涉及具体数据的操作，都必须自己实现
// 所有只跟链表相关的操作，都统一由内核链表提供

linklist init_list(void)
{
    linklist head = calloc(1, sizeof(listnode));
    if (head != NULL)
    {
        // 初始化大结构体中的小结构体list
        // 使之形成一个自己指向自己的循环链表
        INIT_LIST_HEAD(&head->list);
    }
    return head;
}

linklist new_node(int data)
{
    linklist new = calloc(1, sizeof(listnode));
    if (new != NULL)
    {
        new->data = data;
        INIT_LIST_HEAD(&new->list);
    }
    return new;
}

void show(linklist head)
{
    struct list_head *pos;
    linklist p;

    list_for_each(pos, &head->list)
    {
        p = list_entry(pos, listnode, list);
        printf("%d ", p->data);
    }
    printf("\n");
}

void rearrange(linklist head)
{
    struct list_head *pos, *odd = (&head->list)->prev;
    linklist p;

    list_for_each_prev(pos, &head->list)
    {
        p = list_entry(pos, listnode, list);

        if (p->data % 2 == 0)
        {
            list_move_tail(pos, &head->list);
            pos = odd;
        }
        else
            odd = pos;
    }
}

int main(int argc, char **argv)
{
    // 创建空链表
    linklist head = init_list();

    printf("请输入节点个数:");
    int n;
    if (scanf("%d", &n) != 1 || n <= 0)
    {
        printf("请输入正整数，再见！\n");
        exit(0);
    }

    // 依次在链表中插入自然数
    for (int i = 1; i <= n; i++)
    {
        linklist new = new_node(i);

        // 使用内核链表提供的插入算法
        list_add_tail(&new->list, &head->list);
    }
    printf("重排前:");
    show(head);

    rearrange(head);
    printf("重排后:");
    show(head);

    return 0;
}