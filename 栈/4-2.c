#include<stdio.h>
void hanoi(int n,char start,char temp,char dest)
{
    if(n==1)
    {
        printf("圆盘1：%c --> %c\n", start, dest);
        return ;
    }
    hanoi(n-1,start,dest,temp);
    printf("圆盘%d：%c --> %c\n", n, start, dest);
    hanoi(n-1,temp,start,dest);
    

}
int main(int argc, char const *argv[])
{
    hanoi(3,'A','B','C');
    return 0;
}
