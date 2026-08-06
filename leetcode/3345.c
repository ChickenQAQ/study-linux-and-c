#include<stdio.h>
#include<stdbool.h>
int smallestNumber(int n, int t) {
    bool s = false;
    int m[100]={0};
    while(!s)
    {
        int num =1;
        int i =0;
        int tmp=n;
        while(tmp!=0)
        {
            m[i]=tmp%10;
            i++;
            tmp/=10;
        }
        i=i-1;
        for(;i>=0;i--)
        num*=m[i];
        if(num%t==0)
        s=true;
        else
        n++;
    }
    return n;
}
int main(void)
{
    int n = 1;
    int t = 2;
    int res = smallestNumber(n,t);
    printf("%d",res);
    return 0;
}