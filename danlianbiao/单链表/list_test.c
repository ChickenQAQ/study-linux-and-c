#include<stdio.h>
#include <stdlib.h>
#include "list.h"
int main(int argc, char const *argv[])
{
    //1 空链表
    node * mylist = list_init();
    //2放入若干整数
    int n;
    scanf("%d",&n);
    for(int i = 1 ;i<=n;i++)
    {
        //2.1获得新节点
        node *new = new_node(i);
        //2.2将新节点，链入链表中
        add_node_head(mylist,new);
        
    }
    for(int i = 1 ;i<=n;i++)
    {
        //2.1获得新节点
        node *new = new_node(i);
        //2.2将新节点，链入链表中
        add_node_tail(mylist,new);
        
    }
    //3.遍历链表
    list_for_each(mylist);
    //4.销毁链表，释放内存
    list_destroy(mylist);
    return 0;
}
