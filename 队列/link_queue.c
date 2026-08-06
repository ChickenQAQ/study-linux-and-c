#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// 队列的设计
struct node
{
    int data;
    struct node *next;
};

// 链式队列的管理结构体
typedef struct linkqueue
{
    struct node *front;
    struct node *rear;
    int size;
}linkqueue;

linkqueue * init_queue(void)
{
    linkqueue *q = malloc(sizeof(linkqueue));
    if(q != NULL)
    {
        q->front = NULL;
        q->rear = NULL;
        q->size = 0;
    }
    return q;
}

struct node *new_node(int data)
{
    struct node *n = malloc(sizeof(struct node));
    if(n != NULL)
    {
        n->data = data;
        n->next = NULL;
    }
    return n;
}

bool is_empty(linkqueue *q)
{
    return q->size == 0;
}

void enqueue(linkqueue *q, struct node *n)
{
    if(is_empty(q))
        q->front = n;
    else
        q->rear->next = n;
    q->rear = n;
    q->size++;
}


struct node *out_queue(linkqueue *q)
{
    if(is_empty(q))
        return NULL;
            
    // 1.
    struct node *p = q->front;
    
    // 2.3.
    q->front = q->front->next;
    p->next = NULL;
    q->size--;

    // 特殊情况，单独处理
    if(q->size == 0)
        q->rear = NULL;

    return p;
}


void destroy_queue_init(linkqueue *q)
{
    if(is_empty(q))
        return;

    struct node *p;
    while( (p=out_queue(q))  != NULL)
    {
        free(p);
    }
}

int main(int argc, char const *argv[])
{
    // 1. 搞个空队列
    linkqueue *q = init_queue();
    if(q == NULL)
    {
        perror("初始化队列失败");
        return 0;
    }

    // 2. 入队一些数据
    for(int i=1; i<=10; i++)
        enqueue(q, new_node(i));

    // 3. 出队检查顺序
    while(!is_empty(q))
    {
        printf("%d\t", out_queue(q)->data);
    }
    printf("\n");

    // 4. 销毁队列
    destroy_queue_init(q);
    free(q);

    return 0;
}
