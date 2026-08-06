#include <stdio.h>
void home_helper(char *s, void (*p)(int),int n)
{
    printf("AI帮你完成了%s作业\n", s);
    p(n);
}
void p(int n)
{
    if(n==1)
    printf("已成功提交到老师的邮箱\n");
    if(n==2)
    printf("已成功提交到飞秋共享文件夹\n");

}

int main(int argc, char const *argv[])
{
     int n;
    printf("你要做什么作业\n");
    char s[100];
    scanf("%s", &s);
    printf("你要提交到哪里，1是邮箱，2是飞秋\n");
    scanf("%d",&n);
    home_helper(s, p,n) ;
    return 0;
}
