// 接收用户的输入信息，当用户结束程序时，自动统计用户输入的空格数、
// 大小写字母数和其他字符数。
#include <stdio.h>
#include <string.h>
int main()
{
    char s[200];
    int j, k, l, w;
    j = k = l = w = 0;
    fgets(s, 100, stdin);
    int i = 0;
    int size = strlen(s);
    for (int i = 0; i < size; i++)
    {
        if (s[i] >= 65 && s[i] <= 90)
        {
            j++;
            continue;
        }
        else if (s[i] >= 97 && s[i] <= 122)
        {
            k++;
            continue;
        }
        else if (s[i] == 32)
        {
            l++;
            continue;
        }
    }
    w = size - l - j - k - 1;
    printf("该信息中小写字母有%d,大写字母有%d,空格有%d,其他字符有%d", k, j, l, w);
}