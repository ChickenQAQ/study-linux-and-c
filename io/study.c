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
    if(argc != 3)
    {
        printf("Usage: %s <src> <dst>\n",argv[0]);
        exit(0);
    }
    int fd_src = open(argv[1],O_RDONLY,0644);
    int fd_dst = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,0644);
    if(fd_src==-1||fd_dst==-1)
    {
        perror("open() failed");
        exit(0);
    }
    char buf[100];
    int nread,nwrite;
    while(1)
    {
        bzero(buf,100);
        while((nread = read(fd_src,buf,100))==-1&&errno==EINTR)
        {
            sleep(1);
        }
        if(nread == -1)
        {
            perror("read() failed");
            exit(1);
        }
        if(nread == 0)
        {
            break;
        }
        char *p = buf;
        while(nread > 0)
       {
            nwrite = write(fd_dst,p,nread);
            if(nwrite == -1)
            {
                perror("write() failed");
                exit(1);
            }
            p += nwrite;
            nread -= nwrite;
       }
        
    }
   return 0;
}