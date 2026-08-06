#include <stdio.h>
#include <stdlib.h>
struct node
{
    int data;
    struct node *next;
};

typedef struct linkstack
{
    struct node *top;
    int size;
} linkstack;

linkstack *initstack()
{
    linkstack *new = malloc(sizeof(linkstack));
    if (new != NULL)
    {
        new->top = malloc(sizeof(struct node));
        new->top->next = NULL;
        new->size = 0;
    }
    return new;
}
struct node * newnode(int data)
{
    struct node * new = malloc(sizeof(struct node));
    new->data = data;
    new->next = NULL;
    return new;

}
void insert_stack(linkstack *stack,int data)
{
    struct node * new= newnode(data);
    new->next = stack->top->next;
    stack->top->next = new;
    stack->size++;

}
void showstack(linkstack * stack)
{
    struct node *temp = stack->top->next;
    while(temp!=NULL)
    {
        printf("%d\t",temp->data);
        temp=temp->next;
    }
}

void pop(linkstack *stack,int *data)
{
    if(stack == NULL||stack->size==0)
    {
        printf("栈为空\n");
        return;
    }
    struct node * temp = stack->top->next;
    *data = temp->data;
    stack->top->next = temp->next;
    free(temp);
    stack->size--;
}


int main(int argc, char const *argv[])
{
    linkstack *stack = initstack();
    if (stack != NULL)
        printf("初始化空栈链表成功\n");
    insert_stack(stack,1);
    insert_stack(stack,2);
    showstack(stack);
    int m ;
    pop(stack,&m);
    printf("\n");
    printf("%d\n",m);
    showstack(stack);
    return 0;
}
