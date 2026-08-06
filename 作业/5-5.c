#include <stdio.h>

int f(int a)
{
    if (a == 0)
        return 0;
    if (a == 1)
        return 1;

    return f(a - 1) + f(a - 2);
}

int main(int argc, char const *argv[])
{
    int i = 0;
    scanf("%d", &i);
    printf("%d", f(i));
    return 0;
}
