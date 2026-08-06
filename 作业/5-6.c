#include <stdio.h>
#include <string.h>
void stre (char *s,int len)
{
     for (int i = 0; i < len; i++)
    {
        if (s[i] != ' ')
            printf("%c", s[i]);
    }

}
int main(int argc, char const *argv[])
{
    char s[100];
    fgets(s, 100, stdin);
    int len = strlen(s);
    stre(s,len);
    return 0;
}
