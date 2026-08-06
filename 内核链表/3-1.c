#include "kernel_list.h"
#include <stdio.h>
#include <stdlib.h>
// 初始化内核链表
typedef struct node
{
    int data;

    struct list_head list;
} listnode, *linklist;

linklist initnode()
{
    linklist new = malloc(sizeof(listnode));
    if (new != NULL)
    {
        INIT_LIST_HEAD(&new->list);
    }
    return new;
}
linklist newnode(int n)
{
    linklist new = malloc(sizeof(listnode));
    if (new != NULL)
    {
        new->data = n;
        INIT_LIST_HEAD(&new->list);
    }
    return new;
}
// 展示遍历节点
void showlist(linklist head)
{
    struct list_head *pos;
    list_for_each(pos, &head->list)
    {
        struct node *p;
        // entry，需要传入临时链表指针pos，结构表，链表头
        p = list_entry(pos, struct node, list);
        printf("%d\t", p->data);
    }
    printf("\n");
}
void sortlist_reserve(linklist head)
{
    struct list_head *cer = head->list.next;
    for (; cer != &head->list; cer = cer->next)
    {
        for (struct list_head *cerr = cer->next; cerr != &head->list; cerr = cerr->next)
        {
            struct node *prevdata = list_entry(cer, struct node, list);
            struct node *cerdata = list_entry(cerr, struct node, list);
            if ((prevdata->data) > (cerdata->data))
            {
                int temp = prevdata->data;
                prevdata->data = cerdata->data;
                cerdata->data = temp;
            }
        }
    }
    struct node *pos, *nxt;
    list_for_each_entry_safe_reverse(pos, nxt, &head->list, list)
    {
        if (pos->data % 2 == 0)
        {
            list_move_tail(&pos->list, &head->list);
        }
    }
}
int main(int argc, char const *argv[])
{
    linklist head = initnode();
    if (head != NULL)
        printf("初始化内核链表成功\n");
    else
    {
        printf("初始化内核链表失败\n");
    }
    int m;
    printf("你要插入多少数据\n");
    scanf("%d", &m);
    for (int i = 0; i < m; i++)
    {
        int n;
        printf("请输入你要插入的数据\n");
        scanf("%d", &n);
        list_add_tail(&newnode(n)->list, &head->list);
    }
    showlist(head);
    sortlist_reserve(head);
    showlist(head);
    return 0;
}
