#include <stdio.h>
int f(int a, int b, int c)
{
    int max;
    max = a > b ? a : b;
    max = max > c ? max : c;
    return max;
}
int main(int argc, char const *argv[])
{
    int a = 2, b = 3, c = 4;
   int d = f(a, b, c);
    printf("%d", d);
    return 0;
}
