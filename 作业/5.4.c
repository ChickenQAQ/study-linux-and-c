#include <stdio.h>
#include <string.h>
int main(int argc, char const *argv[])
{
    char s[100];
    char ss[100];
    char sums[10000];
    int len1, len2;
    printf("请输入字符串一\n");
    scanf("%s", s);
    printf("请输入字符串二\n");
    scanf("%s", ss);
    len1 = strlen(s);
    len2 = strlen(ss);
    for (int i = 0; i < len1; i++)
    {
        sums[i] = s[i];
    }
    for (int i = 0; i < len2; i++)
    {
        sums[len1 + i] = ss[i];
    }
    sums[len1 + len2] = '\0';
    printf("拼接后的字符串为：%s", sums);

    return 0;
}
