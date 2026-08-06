#ifndef DATATYPE
#define DATATYPE int
#endif
typedef DATATYPE datatype;
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct node
{
    datatype data;

    struct node *prev;
    struct node *next;
} node;

static node *initlist()
{
    node *new = malloc(sizeof(node));
    if (new != NULL)
    {
        new->next = new;
        new->prev = new;
    }
    return new;
}
static node *newnode(datatype n)
{
    node *new = malloc(sizeof(node));
    if (new != NULL)
    {
        new->data = n;
        new->next = new;
        new->prev = new;
    }
    return new;
}
static void insertlist(node *head, node *new)
{
    new->prev = head;
    new->next = head->next;
    head->next->prev = new;
    head->next = new;
}
static void insertlist_last(node *head, node *new)
{
    new->prev = head->prev;
    head->prev->next =new;
    head->prev = new;
    new->next = head;
}
static void showlist(node *head, void (*handler)(datatype *n))
{
    for (node *temp = head->next; temp != head; temp = temp->next)
    {
        handler(&temp->data);
    }
}
static bool deletelist(node *head, datatype n, bool (*equaldata)(datatype *temp, datatype *n))
{
    node *crt = head;
    for (node *temp = head->next; temp != head; temp = temp->next)
    {

        if (equaldata(&temp->data, &n))
        {
            crt->next = temp->next;
            temp->next->prev = crt;
            free(temp);
            return true;
        }
        crt = temp;
    }
    return false;
}
static void findlist(node *head, datatype n, bool (*equaldata)(datatype *, datatype *), void (*showdata)(datatype *n))
{
    for (node *temp = head->next; temp != head; temp = temp->next)
    {
        if (equaldata(&temp->data, &n))
        {
            printf("找到节点\n");
            showdata(&temp->data);
            return;
        }
    }
    printf("未找到节点\n");
}
static void destroylist(node * head)
{
    if (head == NULL) return;
    node *p = head->next;
    while (p != head)
    {
        node *q = p;
        p = p->next;
        free(q);
    }
    free(head);
}