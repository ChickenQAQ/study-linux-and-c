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
    if(argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR | O_CREAT, 0666);
    if(fd == -1)
    {
        perror("open");
        return 1;
    }

    off_t size = lseek(fd, 10000, SEEK_END);
    if(size == -1)
    {
        perror("lseek");
        return 1;
    }
    write(fd, "111", 3);
    close(fd);
    return 0;
}