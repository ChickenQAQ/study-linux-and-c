#include <stdio.h>
#include <math.h>
void transform(double source[], double target[], int num, double (*p)(double))
{
    for (int i = 0; i < num; i++)
    {
        target[i] = p(source[i]);
    }
}
int main(int argc, char const *argv[])
{
    int n, j;
    printf("请输入数组长度n\n");
    scanf("%d", &n);
    double source[n];
    double target[n];
    for (int i = 0; i < n; i++)
    {
        printf("请输入第%d号元素的值", i + 1);
        scanf("%lf", &source[i]);
    }
    printf("你要转成cos还是sin,如果转为sin则输入1,cos则输入0\n");
    while (1)
    {
        int q = scanf("%d", &j);
        if (q != 1)
        {
            while (getchar() != '\n')
                printf("请输入正确的值\n");
        }
        if (j == 0)
        {
            transform(source, target, n, cos);
            break;
        }
        else if (j == 1)
        {
            transform(source, target, n, sin);
            break;
        }
        else
            printf("请输入正确的值\n");
    }
    for (int i = 0; i < n; i++)
        printf("%lf\n", target[i]);

    return 0;
}
