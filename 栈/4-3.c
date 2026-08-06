#include<stdio.h>
#include<stdlib.h>
int N;
int cnt = 0;
void dfs(int start,int stack[],int top,int res[],int pos)
{
    if(start>N&&top==-1)
    {
        cnt++;
        for(int i =0;i<pos;i++)
        printf("%d ",res[i]);
        printf("\n");
        return;
    }
    if(start<=N)
    {
        int new_start[100]={0};
        int new_out[100]={0};
        for(int i =0;i<=top;i++)
            new_start[i] = stack[i];
        for(int i =0;i<pos;i++)
            new_out[i]=res[i];
        new_start[top+1]=start;
        dfs(start+1,new_start,top+1,new_out,pos);
    }
    if(top!=-1)
    {
        int new_st[100] = {0};
        int new_out[100] = {0};
        for(int i = 0; i <= top; i++)
            new_st[i] = stack[i];
        for(int i = 0; i < pos; i++)
            new_out[i]=res[i];
           
        new_out[pos]=new_st[top];
        dfs(start,new_st,top-1,new_out,pos+1);
    }
}
int main(int argc, char const *argv[])
{
    printf("请输入车厢数\n");
    scanf("%d",&N);
    int stack[100]={0};
    int res[100]={0};
    dfs(1,stack,-1,res,0);
    return 0;
}
