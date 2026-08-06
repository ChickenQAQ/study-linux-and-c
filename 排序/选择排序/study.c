#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
void show(int data[])
    {
        int i;
        for(i=0;i<10;i++)
        {
            printf("%d\t",data[i]);
        }
        printf("\n");
    }
    void swap (int *a,int *b)
    {
        int temp = *a;
        *a = *b;
        *b = temp;
    }
    void selectionSort(int data[],int len)
    {
        int i,j,min;
        for(i=0;i<len;i++)
        {
            min = i;
            for(j=i+1;j<len;j++)
            {
                if(data[j]<data[min])
                {
                    min = j;
                }
            }
        swap(&data[i],&data[min]);
        }
    }
int main()
{
    int data[10] = {10,9,8,7,6,5,4,3,2,1};
    selectionSort(data,10);
    show(data);
    return 0;
}