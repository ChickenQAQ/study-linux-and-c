// 双向循环链表
#include <stdio.h>
#include <stdlib.h>
typedef struct node
{
    int data;

    struct node *prev;
    struct node *next;
} node;
// 带头节点版本的双向循环链表的初始化
node *initlist()
{
    node *head = malloc(sizeof(node));
    if (head != NULL)
    {
        head->next = head;
        head->prev = head;
    }
    return head;
}
// 初始化新数据
node *newnode(int n)
{
    node *new = malloc(sizeof(node));
    if (new != NULL)
    {
        new->next = new;
        new->prev = new;
        new->data = n;
    }
    return new;
}

// 头插法插入数据
void insert_data_head(node *list, int n)
{
    node *new = newnode(n);
    new->prev = list;
    new->next = list->next;
    list->next->prev = new;
    list->next = new;
}
// 尾插法
void insert_data_last(node *list, int n)
{
    node *new = newnode(n);
    new->prev = list->prev;
    new->next = list;
    list->prev->next = new;
    list->prev = new;
}
// 删除链表指定节点
bool delete_node(node *list, int data)
{
    if (list == NULL)
        return false;
    node *p = list->next;
    while (p != list)
    {
        if (p->data == data)
        {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            p->next = NULL;
            p->prev = NULL;
            free(p);
            return true;
        }
        p = p->next;
    }
    return false;
}

// 遍历输出链表
void showlist_head(node *list)
{
    for (node *temp = list->next; temp != list; temp = temp->next)
        printf("%d", temp->data);
    printf("\n");
}
// 从尾到前遍历
void showlist_last(node *list)
{
    for (node *temp = list->prev; temp != list; temp = temp->prev)
        printf("%d", temp->data);
    printf("\n");
}
int main(int argc, char const *argv[])
{
    node *list = initlist();
    if (list != NULL)
    {
        printf("初始化链表成功\n");
    }
    for (int i = 1; i <= 5; i++)
        insert_data_last(list, i);
    if (delete_node(list, 2))
        printf("删除节点成功\n");
    else
        printf("删除节点失败\n");
    showlist_head(list);
    return 0;
}
