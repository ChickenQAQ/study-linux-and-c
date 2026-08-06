#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
    int data;

    struct node *next;
} node;
// 初始化链表
node *initlist()
{
    return NULL;
}
// 创建新节点
node *newnode(int n)
{
    node *new = malloc(sizeof(node));
    new->data = n;
    new->next = NULL;
    return new;
}
// 头插入链表
void insert_listhead(node **list, node *n)
{
    if (*list == NULL)
    {
        *list = n;
        n->next = *list;
    }
    else
    {
        node *tail = *list;
        while (tail->next != *list)
            tail = tail->next;
        tail->next = n;
        n->next = *list;
        *list = n;
    }
}
// 遍历链表
void showlist(node *list)
{
    node *temp =list;
    if (list == NULL)
    {
        printf("链表为空\n");
        return;
    }
    int count = 0;
    do
    {
        printf("%d\t", temp->data);
        temp = temp->next;
        count++;
        if (count > 100)
        { // 防止死循环
            printf("链表可能已损坏！\n");
            return;
        }
    } while (temp != list);
    printf("\n");
}

// 删除链表节点
bool delete_listdata(node **list, int n)
{
    if (*list = NULL)
        return false;
    node *p = NULL;
    node *temp = *list;
    do
    {
        if (temp->data == n)
            break;
        p = temp;
        temp = temp->next;
    } while (temp != *list);
    if (temp->data != n)
        return false;
    //如果只有一个节点,直接free掉
    if (temp == *list && temp->next == *list)
    {
        free(temp);
        *list = NULL;
        return true;
    }
    //如果是头节点的情况。
    if (temp == *list)
    {
        node *tail = *list;
        while (tail->next != *list)
        tail = tail->next;
        tail->next = temp->next;
        *list = temp->next;
    }
    else
    {
        p->next = temp->next;
    }
    free(temp);
    return true;
}

// 死亡游戏
void daygame(node **list, int n)
{
    //如果链表为空
    if(*list ==NULL||n<=0)
    return;
    //如果只循环一次，遍历整个链表，都存活
    if(n<=2)
    {
        showlist(*list);
        return;
    }
    node *p = *list;
    while(p->next!=*list)
    p=p->next;//找到尾指针
    node *c = *list;//c为p节点的下一个节点
    int rest = n;
    while (rest>2)
    {
        for(int i =1;i<3;i++)
        {
            //报数的时候节点往后移
            p=c;
            c=c->next;
        }
        //报数到三时将c节点free掉
        p->next = c->next;
        //当c为头节点时需要变更头节点，不能直接free
        if(c==*list)
        *list=c->next;
        free(c);
        c=p->next;
        rest--;
    }
    showlist(*list);
}

int main(int argc, char const *argv[])
{
    int n;
    printf("请输入人数n\n");
    scanf("%d", &n);
    node *list = initlist();
    for (int i = n; i > 0; i--)
        insert_listhead(&list, newnode(i));
    showlist(list);
    daygame(&list, n);
    return 0;
}
