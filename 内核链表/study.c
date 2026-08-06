#include "kernel_list.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
typedef struct student
{
    int id;
    char name[20];
    int age;
} student;

// 设计自己的大结构体
typedef struct node
{
    student data;
    struct list_head list;
} listnode, *linklist;
linklist initlist()
{
    linklist head = malloc(sizeof(listnode));
    if (head != NULL)
    {
        INIT_LIST_HEAD(&head->list);
    }
    return head;
}
//创建新节点
linklist newnode(int n, char *name, int age)
{
    linklist new = malloc(sizeof(listnode));
    if (new != NULL)
    {
        INIT_LIST_HEAD(&new->list);
        new->data.id = n;
        new->data.age = age;
        strcpy(new->data.name, name);
        return new;
    }
}
//寻找节点
linklist findlist(linklist head, student stu)
{
    linklist pos = malloc(sizeof(listnode));
    list_for_each_entry(pos, &head->list, list)
    {
        if (pos->data.id == stu.id&&pos->data.age==stu.age&&strcmp(pos->data.name,stu.name)==0)
        {
            return pos;
        }
    }
    printf("没有找到想找的数据\n");
    return NULL;
}
//展示遍历节点
void showlist(linklist head)
{
    struct list_head *pos;
    list_for_each(pos, &head->list)
    {
        struct node *p;
        // entry，需要传入临时链表指针pos，结构表，链表头
        p = list_entry(pos, struct node, list);
        printf("%d\t%s\t%d\n", p->data.id, p->data.name, p->data.age);
    }
    printf("\n");
}
// 删除节点
void deletedata(linklist head, student stu)
{
    linklist p = findlist(head, stu);
    if(p==NULL)
        return;
    printf("删除节点成功\n");
    list_del(&p->list);
    free(p);
    printf("删除链表成功\n");
}
int main(int argc, char const *argv[])
{
    linklist head = initlist();
    if (head != NULL)
        printf("初始化成功\n");
        linklist ZHANGSAN = newnode(1, "张三", 18);
        linklist LIUAIER = newnode(2, "李四", 18);
        linklist WANGWU = newnode(3, "王五", 18);
        list_add_tail(&ZHANGSAN->list, &head->list);
        list_add_tail(&LIUAIER->list, &head->list);
        list_add_tail(&WANGWU->list, &head->list);
    showlist(head);
    // 删除链表中的某一个节点
    linklist p = findlist(head,WANGWU->data);
    printf("检查是否能查到数据：%d\t%s\t%d\n", p->data.id, p->data.name, p->data.age);
    deletedata(head,WANGWU->data);
    showlist(head);
    return 0;
}
