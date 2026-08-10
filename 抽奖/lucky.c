#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

int main()
{
    FILE *fp=fopen("英雄名单.txt","r");
    char (*p)[100] = calloc(100,60 * sizeof(char));
    int i = 0;
    for(i = 0; i < 100; i++)
    {
        if(fgets((char *)(p+i),60,fp) == NULL)
        {
            break;
        }
        strtok(p[i],"\n");
    }
    srand(time(NULL));
    while(1)
    {
        int a = rand() % i;
        printf("\r                                          ");
        printf("\r%s",p[a]);
        fflush(stdout);
        usleep(200*1000);
    }
    free(p);
    fclose(fp);
}