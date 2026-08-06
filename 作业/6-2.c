#include <stdio.h>

void f(void)
{
    static int c = 1;
    printf("%d\n", c);
    c++;
}
int main(int argc, char const *argv[])
{
    f();
    f();
    return 0;
}
