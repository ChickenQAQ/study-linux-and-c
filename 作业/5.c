#include <stdio.h>
void f(int a[], int b[], int lenA, int lenB)
{
    int sum = lenA > lenB ? lenB : lenA;
    int sum1 = lenA > lenB ? lenA : lenB;
    int c[sum];
    for (int i = 0; i < sum; i++)
    {
        c[i] = a[i] + b[i];
    }
    if (lenB > lenA)
    {
        for (int n = sum; n < lenB; n++)
        {
            c[n] = b[n];
        }
    }
    else
    {
        for (int n = sum; n < lenA; n++)
        {
            c[n] = a[n];
        }
    }
    for (int j = 0; j < sum1; j++)
        printf("%d,", c[j]);
}

int main()
{
    int a[4] = {1, 2, 3, 4};
    int b[6] = {1, 2, 3, 4, 5, 6};
    f(a, b, 4, 6);
}