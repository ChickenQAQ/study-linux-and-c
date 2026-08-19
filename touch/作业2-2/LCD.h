// #ifndef __LCD_H__
// #define __LCD_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/types.h>

// LCD对象结构体
struct lcdinfo
{
    int lcdfd;

    int width; // 宽度
    int height; // 高度
    int bpp;
    int pitch;
    int screensize;

    unsigned *fbmem;
};

extern bool lcd_init(const char *dev_name, struct lcdinfo *p);
extern void release_lcd(struct lcdinfo *p);


// #endif // __LCD_H__