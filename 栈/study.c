#include <stdio.h>
#include <stdlib.h>
// 顺序栈
struct seqStack
{
    int *data;
    int size;
    int top;
};
struct seqStack *initStack(int size)
{
    struct seqStack *stack = malloc(sizeof(struct seqStack));
    if (stack == NULL)
    {
        free(stack);
        return NULL;
    }
    stack->data = calloc(size, sizeof(int));
    stack->size = size;
    stack->top = -1;
    return stack;
}
bool isFull(struct seqStack *stack)
{
    return stack->top == stack->size - 1;
}
bool isEmpty(struct seqStack *stack)
{
    return stack->top == -1;
}
bool push(struct seqStack *stack, int data)
{
    if (isFull(stack))
        return false;
    stack->data[++stack->top] = data;
    return true;
}
bool top(struct seqStack *stack, int *m)
{
    if (isEmpty(stack))
        return false;
    *m = stack->data[stack->top];
    return true;
}
bool pop(struct seqStack *stack, int *m)
{
    if (!top(stack, m))
        return false;
    stack->top--;
}

int main(int argc, char const *argv[])
{
    // 初始化一个空的顺序栈
    struct seqStack *stack = initStack(100);
    printf("请输入10进制数\n");
    int n;
    scanf("%d",&n);
    while(n!=0)
    {
        push(stack,n%16);
        n/=16;
    }
    int m;
    printf("16进制数为：0x");
    while(!isEmpty(stack))
    {
        pop(stack,&m);
        if(m<10)
        printf("%d",m);
        else
        printf("%c",m-10+'A');
    }
    printf("\n");
}
