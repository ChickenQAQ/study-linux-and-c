#include <stdio.h>

void f(int m, int n, int a[][n], int b[][n])
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            b[i][j] = a[i][j];
        }
    }
}
int main(int argc, char const *argv[])
{
    int a[3][2] = {{1, 2}, {2, 3}, {3, 4}};
    int b[3][2];
    f(3, 2, a, b);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            printf("%d,", b[i][j]);
        }
    }
    return 0;
}
