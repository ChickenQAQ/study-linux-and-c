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
int main(int argc, char *argv[])
{
    if(argc !=2)
    {
        printf("Usage: %s <file>\n",argv[0]);
        exit(0);
    }
    int fd;
    fd = open(argv[1],O_RDONLY,0644);
    if(fd==-1)
    {
        perror("open() failed");
        exit(0);
    }
    char buf[100];
    while(1)
    {
        bzero(buf,100);
        if(read(fd,buf,100)==0)
        {
            break;
        }
        printf("%s",buf);
    }
   lseek(fd,0,SEEK_SET);
    while(1)
    {
        bzero(buf,100);
        if(read(fd,buf,100)==0)
        {
            break;
        }
        printf("%s",buf);
    }
    return 0;
}
