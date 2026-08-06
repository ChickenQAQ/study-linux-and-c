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
int  getsize(queue * que)
{
    return (que->rear-que->front+que->capacity)%que->capacity;
}
int main(int argc, char const *argv[])
{
    queue * que = initQueue(100);
    int inque [101] ={0};
    int n;
    printf("请输入景观数量\n");
    scanf("%d",&n);
    int maxlen =  0 ;
    for(int i =0 ;i<n;i++)
    {
        printf("输入景观编号\n");
        int k ;
        scanf("%d",&k);
        enQueue(que,k);
        showQueue(que);
        while(inque[k])
        {
            int N;
            outQueue(que,&N);
            inque[k]=0;
        }
        inque[k]=1;
    }
    maxlen=getsize(que);
    showQueue(que);
    printf("最长连续观光为%d",maxlen);
    return 0;
}
