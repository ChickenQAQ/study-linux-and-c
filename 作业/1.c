#include <stdio.h>
#include <stdlib.h>
int main(int argc, char const *argv[])
{
    int *i = malloc(50 * sizeof(int));
    for (int n = 0; n < 50; n++)
    {
        i[n] = n;
        printf("%d", i[n]);
    }
    return 0;
}
