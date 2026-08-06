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
struct node *newnode(int data)
{
    struct node *new = malloc(sizeof(struct node));
    new->data = data;
    new->next = NULL;
    return new;
}
void insert_stack(linkstack *stack, int data)
{
    struct node *new = newnode(data);
    new->next = stack->top->next;
    stack->top->next = new;
    stack->size++;
}
void showstack(linkstack *stack)
{
    struct node *temp = stack->top->next;
    while (temp != NULL)
    {
        printf("%d\t", temp->data);
        temp = temp->next;
    }
}

void pop(linkstack *stack, int *data)
{
    if (stack == NULL || stack->size == 0)
    {
        printf("栈为空\n");
        return;
    }
    struct node *temp = stack->top->next;
    *data = temp->data;
    stack->top->next = temp->next;
    free(temp);
    stack->size--;
}

int main(int argc, char const *argv[])
{
    char s[] = "0123456789abcdef";
    linkstack *stack = initstack();
    if (stack != NULL)
        printf("初始化栈链表成功\n");
    int n;
    bool q = false;
    printf("请输入十进制数\n");
    scanf("%d", &n);
    int k = n;
    if (n < 0)
    {
        n = -n;
        q = true;
    }
    while (n > 0)
    {
        insert_stack(stack, n % 16);
        n /= 16;
    }
    char res[stack->size + 1];
    int i = 0;
    while (stack->size != 0)
    {
        int tmp;
        pop(stack, &tmp);
        res[i++] = s[tmp];
    }
    res[i] = '\0';
    if (q)
        printf("10进制数%d转换为16进制数的值为-%s\n", k, res);
    else
        printf("10进制数%d转换为16进制数的值为%s\n", k, res);
    return 0;
}
