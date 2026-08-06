#include<stdio.h>
int main(int argc, char const *argv[])
{
    char s[] = "0123456789abcdef";
    int x = 12345678;
    int tmp[8];
    for(int i =7;i>=0;i--)
    {
        tmp[i]=x%16;
        x=x/16;
    }
    char res[9];
    for(int i=0;i<8;i++)
    {
        res[i]=s[tmp[i]];
    }
    res[8]='\0';
    printf("%s\n",res);
    return 0;
}
