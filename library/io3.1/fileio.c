#include "fileio.h"
#include<errno.h>


int my_read(int fd, void *buf, size_t count)
{
    int ret = read(fd, buf, count);
    if (ret == -1)
    {
        perror("read");
        return -1;
    }
    return ret;
}

int my_write(int fd, const void *buf, size_t count)
{
    int ret = write(fd, buf, count);
    if (ret == -1)
    {
        perror("write");
        return -1;
    }
    return ret;
}

int my_open(const char *pathname, int flags, mode_t mode)
{
    int fd = open(pathname, flags, mode);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }
    return fd;
}
int my_close(int fd)
{
    int ret = close(fd);
    if (ret == -1)
    {
        perror("close");
        return -1;
    }
    return ret;
}
int my_lseek(int fd, off_t offset, int whence)
{
    int ret = lseek(fd, offset, whence);
    if (ret == -1)
    {
        perror("lseek");
        return -1;
    }
    return ret;
}
int my_stat(const char *pathname, struct stat *buf)
{
    int ret = stat(pathname, buf);
    if (ret == -1)
    {
        perror("stat");
        return -1;
    }
    return ret;
}