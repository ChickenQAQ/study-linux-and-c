#include <stdio.h>
struct rq
{
    int year;
    int month;
    int day;
};

int f(int year, int month, int day)
{
    int num = 0;
    for (int i = 1; i <= month; i++)
    {
        if (year % 4 == 0)
        {
            if (i == 2)
                num += 29;
        }
        else
        {
            if (i == 2)
                num += 28;
        }
        if (i != 2)

        {
            if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i == 12)
                num += 31;
            else
            {
                num += 30;
            }
        }
    }
    return num;
}

int main(int argc, char const *argv[])
{
    struct rq n;

    printf("请输入今年年份\n");
    scanf("%d", &n.year);
    printf("现在是几月份\n");
    while (1)
    {
        int i = scanf("%d", &n.month);
        if (n.month < 0 || n.month > 12 || i != 1)
        {
            printf("请输入正确月份\n");
            continue;
        }
        break;
    }
    printf("现在是几号\n");
    while (1)
    {
        int i = scanf("%d", &n.day);
        if (n.day < 0 || n.day > 31 || i != 1)
        {
            printf("请输入正确日期\n");
            continue;
        }

        break;
    }

    int num = f(n.year, n.month, n.day);
    printf("今天是今年的第%d天", num);
    return 0;
}
