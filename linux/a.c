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
int main(void)
{
    int a = -3;
    int fd = open("a.bin",O_RDWR|O_CREAT|O_TRUNC);
    write(fd, &a,4);
    close(fd);
}