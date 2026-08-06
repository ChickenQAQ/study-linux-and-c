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
    int fd = open("a.txt",O_RDWR);
    int flag_val = fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,flag_val|O_NONBLOCK);
}