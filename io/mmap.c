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
    int fd = open("a.txt",O_RDWR);
    if(fd==-1)
    {
        perror("open() failed");
        exit(1);
    }
}