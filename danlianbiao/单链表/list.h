#ifndef __LIST_H
#define __LIST_H
#include<stdio.h>
#include <stdlib.h>
typedef struct node
{
    int data;

    struct node *next;
}node,*list;
node * list_init();
node * new_node (int n);
void add_node_head(node* head,node *new);
void add_node_tail(node* head,node *new);
void list_for_each(node* head);
void list_destroy(node* head);
#endif