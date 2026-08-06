#include <stdio.h>
#include <string.h>
int main(int argc, char const *argv[])
{
    char s[100];
    printf("请输入字符串\n");
    scanf("%s", s);
    char ch[100];
    int i, q;
    q = 0;
    for (i = 0; i < strlen(s); i++)
    {
        for (int j = i + 1; j < strlen(s); j++)
        {
            if (s[i] == s[j] && s[i] != '0' && s[j] != '0')
            {
                s[j] = '0';
            }
        }
        if (s[i] != '0')
        {
            ch[q++] = s[i];
        }
    }
    ch[i] = '\0';
    printf("%s", ch);

    return 0;
}
