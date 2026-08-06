#include <stdio.h>

int main(void)
{
    int N, sum;
    sum = 0;
    printf("请输入数组有几个元素\n");
    scanf("%d", &N);
    int a[N];
    for (int i = 0; i < N; i++)
    {
        printf("请输入第%d号元素的值", i + 1);
        scanf("%d", &a[i]);
    }
    for (int i = 0; i < N; i++)
    {
        int q = a[i];
        for (int j = i + 1; j < N; j++)
        {
            if (j == i + 1 && a[i] > (a[i] + a[j]))
            {
                if (q > sum)
                    sum = q;
            }

            q = q + a[j];
            if (q > sum)
                sum = q;
        }
    }
    printf("子数组之和最大是%d",sum);
}