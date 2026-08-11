#include <stdio.h>
#include<pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    if(argc !=3)
    {
        printf("Usage: %s <file1> <file2> \n",argv[0]);
        exit(1);
    }
    else
    {
        FILE *fp1 = fopen(argv[1],"r");
        if(fp1 == NULL)
        {
            printf("Error opening file %s: %s\n",argv[1],strerror(errno));
            exit(1);
        }
        FILE *fp2 = fopen(argv[2],"w");
        if(fp2 == NULL)
        {
            printf("Error opening file %s: %s\n",argv[2],strerror(errno));
            exit(1);
        }
        char buffer[1024];
        while(fgets(buffer,1024,fp1))
        {
            fputs(buffer,fp2);
        }
        fclose(fp1);
        fclose(fp2);
    }
}