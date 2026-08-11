#include <stdio.h>
#include<pthread.h>
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
    int fd = open("1.txt", O_RDWR|O_CREAT, 0644);
    if(fd==-1)
    {
        perror("open");
        exit(1);
    }
    dup2(fd,1);
    printf("hello world\n");
    close(fd);
    return 0;
}