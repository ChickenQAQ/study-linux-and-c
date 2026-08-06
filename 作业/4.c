#include <stdio.h>
int main(int argc, char const *argv[])
{
    int n;
    scanf("%d", &n);
    for (int i = 1;; i++)
    {
        if (n % 2 == 0)
        {
            n= n / 2;
        }
        else
        {
            n = 3*n+1;
        }
        if(n==4||n==2||n==1)
        {
            printf("变换次数为%d",i);
            break;
        }
    }
}
