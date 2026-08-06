#include<stdio.h>
#include <stdlib.h>
#include "list.h"
node * list_init()
{
    node * head=malloc(sizeof(node));
    if(head!=NULL)
    {
        head->next = NULL;
    }
    return head;
}
node * new_node (int n)
{
    node *new = malloc(sizeof(node));
    if(new!=NULL)
    {
        new->data = n;
        new->next = NULL;
    }
    return new;
}
void add_node_head(node* head,node *new)
{
    new->next = head->next;
    head->next = new;
}
void add_node_tail(node* head,node *new)
{
    node * tail = head;
    for(;tail->next!=NULL;tail=tail->next);
    new->next=tail->next;
    tail->next=new;
}
void list_for_each(node* head)
{
    for(node * temp = head->next;temp!=NULL;temp=temp->next)
    {
        printf("%d\t",temp->data);
    }
    printf("\n");
}
void list_destroy(node* head)
{
    node * temp;
    while(head!=NULL)
    {
        temp=head ->next;
        free(head);
        head = temp;
    }
}