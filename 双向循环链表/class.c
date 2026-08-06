#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct node
{
    int data;
    struct node *next;
    struct node *prev;
}node;

node * initlist(void)
{
    node * head = (node *)malloc(sizeof(node));
    if(head == NULL)
    {
        printf("内存分配失败\n");
        return NULL;
    }
    else
   { 
    head->next = head;
    head->prev = head;
    }
    return head;
}
node * newnode(int data)
{
    node * newnode = (node *)malloc(sizeof(node));
    if(newnode == NULL)
    {
        printf("内存分配失败\n");
        return NULL;
    }
    else
    {
        newnode->data = data;
    }
    return newnode;
}
void insert_list_head(node * head,int data)
{
    node * new = newnode(data);
    new->next = head->next;
    new->prev = head;
    head->next->prev = new;
    head->next = new;
}
void insert_list_last(node * head,int data)
{
    node * new = newnode(data);
    new->next = head;
    new->prev = head->prev;
    head->prev->next = new;
    head->prev = new;
}
void showlist(node * head)
{
    node * p = head->next;
    while(p!=head)
    {
        printf("%d\t",p->data);
        p = p->next;
    }
    printf("\n");
}
bool delete_node(node * head,int data)
{
    if(head == NULL)
        return false;
    node * p = head->next;
    while(p!=head)
    {
        if(p->data == data)
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
void destroylist_leavehead(node * head)
{
    node * p = head->next;
    while(p!=head)
    {
        node * q = p;
        p = p->next;
        free(q);
    }
}
void destroylist(node * head)
{
    destroylist_leavehead(head);
    free(head);
}
int main(void)
{
    node * head = initlist();
    if(head!=NULL)
        printf("初始化成功\n");
    insert_list_head(head,100);
    insert_list_last(head,200);
    showlist(head);
    delete_node(head,100);
    showlist(head);
    destroylist(head);

    return 0;
    
}