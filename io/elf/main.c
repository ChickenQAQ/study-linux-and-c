#include<stdio.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include"a.h"

int main(int argc, char *argv[])
{
    FILE * fp =fopen("config","r");
    if(fp==NULL)
    {
        printf("file open error\n");
        return 0;
    }
    char *line = calloc(100,sizeof(char));
    char **lib = calloc(100,sizeof(char*));
    int i =0;
    while(fgets(line,100,fp)!=NULL)
    {
        lib[i] = calloc(100, sizeof(char)); //给这一行分配内存
        sscanf(line,"%s",lib[i]);
        i++;
    }
    fclose(fp);
    for(int j =0;j<i;j++)
    {
        printf("lib:%s\n",lib[j]);
        void *handle = dlopen(lib[j],RTLD_LAZY);
        if(!handle)
        {
            printf("dlopen error:%s\n",dlerror());
            return 0;
        }
        void (*func)(void);
        func = dlsym(handle,"func");
        if(!func)
        {
            printf("dlsym error:%s\n",dlerror());
            return 0;
        }
        func();
    }
    free(line);
    free(lib);
    return 0;
}
