#include <stdbool.h>
#include<stdio.h>
#include <stdlib.h>
typedef struct seqQueue
{
    int *data;
    int capacity;
    int front;
    int rear;
}queue;

queue * initQueue(int size)
{
    queue * new = malloc(sizeof(queue));
    if(new!=NULL)
    {
        new->data=malloc(size * sizeof(int));
        if(new->data!=NULL)
        {
            new->front = new->rear = 0;
            new->capacity = size;
        }
    }
}
bool isFull(queue * que)
{
    return que->rear+1==que->front;
}
bool enQueue(queue * que,int data)
{
    if(isFull(que))
        return false;
    que->data[que->rear]=data;
    que->rear=(que->rear+1)%que->capacity;
    return true;
}
bool isEmpty(queue * que)
{
    return que->rear == que->front;
}
bool outQueue(queue * que,int *n)
{
    if(isEmpty(que))
    return false;
    *n = que->data[que->front];
    que->front =(que->front+1)%que->capacity;
}
void showQueue(queue * que)
{
    if(isEmpty(que))
    return;
    for(int i=que->front;i!=que->rear;i=(i+1)%que->capacity)
    printf("%d\t",que->data[i]);
    printf("\n");
}

int main(int argc, char const *argv[])
{
    queue * que = initQueue(10);
    if(que!=NULL)
    printf("初始化空队列成功\n");
    for(int i = 0;i<5;i++)
    {
        printf("请输入你要入队的数据\n");
        int n;
        scanf("%d",&n);
        enQueue(que,n);
    }
    showQueue(que);
    int k;
    outQueue(que,&k);
    showQueue(que);

    return 0;
}
