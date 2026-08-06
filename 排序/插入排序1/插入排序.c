#include <stdio.h>
#include <stdlib.h>
int main()
{
    int a[10] = {1, 3, 2, 4, 5, 6, 7, 8, 9, 0};
    int i,j,temp;
    for(i=1;i<10;i++)
    {
        temp = a[i];
        for(j=i-1;i>=0;j--)
        {
            if(a[j]>temp)
            {
                a[j+1] = a[j];
                a[j] = temp;
            }
            else
            {
                break;
            }
        }
    }
    for(i=0;i<10;i++)
    {
        printf("%d\t",a[i]);
    }

    printf("\n");
    return 0;
}