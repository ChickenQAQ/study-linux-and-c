#ifndef JPG_H
#define JPG_H
#include<stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdbool.h>
struct jpg_size
{
    int width;
    int height;
    int rgb; // 0:RGB 1:BGR
};
struct lcd_size
{
    int width;
    int height;
    int bpp;
};

char *jpg2rgb(const char *jpgdata, 
    size_t jpgsize,struct jpg_size *jpginfo);
bool showjpg(char *p, struct lcd_size *lcdinfo,
            struct jpg_size *jpginfo,char * jpg);

#endif // JPG_H