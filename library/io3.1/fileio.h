#ifndef FILEIO_H
#define FILEIO_H
#include<stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int my_read(int fd, void *buf, size_t count);
int my_write(int fd, const void *buf, size_t count);
int my_open(const char *pathname, int flags, mode_t mode);
int my_close(int fd);
int my_lseek(int fd, off_t offset, int whence);
int my_stat(const char *pathname, struct stat *buf);
#endif // FILEIO_H