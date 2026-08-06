#include <stdio.h>
#include <stdlib.h>
typedef struct node
{
    int data;

    struct node *prev;
    struct node *next;

} node;
// 建立带头节点的链表
node *initlist(void)
{
    node *head = malloc(sizeof(node));
    if (head != NULL)
    {
        head->next = head;
        head->prev = head;
    }
    return head;
}
// 建立新节点
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
void insert_data_head(node *head, node *data)
{

    data->prev = head;
    data->next = head->next;
    head->next->prev = data;
    head->next = data;
}
// 尾插法插入数据
void insert_data_last(node *head, node *data)
{

    data->next = head;
    data->prev = head->prev;
    head->prev->next = data;
    head->prev = data;
}
//重排前先将原链表按从小到大升序排序
void list_sort_asc(node *head)
{
    if (head->next == head)
        return;
    node *p;
    node *tail = head;
    // 记录最后一次发生交换的位置，优化；或者改成标准写法
    while(tail->next != head)
    {
        node *last = head->next;
        for (p = head->next; p->next != tail; p = p->next)
        {
            if (p->data > p->next->data)
            {
                int tmp = p->data;
                p->data = p->next->data;
                p->next->data = tmp;
                last = p->next;
            }
        }
        tail = last;
    }
}
// 算法:将其奇偶重排输出
void sort(node *head)
{
    list_sort_asc(head);
    node *p = head->prev;
    node * next_p;
    while (p != head)
    {
        next_p = p->prev;
        if (p->data % 2 == 0)
        {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            p->next = NULL;
            p->prev = NULL;
            insert_data_last(head, p);
        }
        p = next_p;
    }
}
void showlist(node *list)
{
    for (node *temp = list->next; temp != list; temp = temp->next)
        printf("%d\t", temp->data);
    printf("\n");
}

int main(int argc, char const *argv[])
{
    int n ;
    node *list = initlist();
    if(list!=NULL)
    {
        printf("初始化成功\n");
    }
    printf("请输入链表的元素个数（自然数）\n");
    scanf("%d",&n);
    for (int i = 0; i < n; i++)
    {
        int q;
        printf("请依次输入链表元素\n");
        scanf("%d",&q);
        insert_data_last(list, newnode(q));
    }
    showlist(list);
    sort(list);
    showlist(list);
    return 0;
}
