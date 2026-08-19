#include "lcd.h"

bool lcd_init(const char *dev_name, struct lcdinfo *p)
{
    // 0. 判断参数合法性
    if(dev_name == NULL || p == NULL)
    {
        return false;
    }

    // 1. LCD设备资源的准备
    printf("[%s][%d]\n", __FILE__, __LINE__);
    p->lcdfd = open(dev_name, O_RDWR);
    if(p->lcdfd < 0)
    {
        perror("open lcd");
        return false;
    }

    // 2. 获取LCD硬件参数
    struct fb_var_screeninfo vinfo;
    if(ioctl(p->lcdfd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror("ioctl");
        return false;
    }

    printf("[%s][%d]\n", __FILE__, __LINE__);
    p->width  = vinfo.xres;
    p->height = vinfo.yres;
    p->bpp    = vinfo.bits_per_pixel;
    p->pitch  = vinfo.xres * vinfo.bits_per_pixel / 8;
    p->screensize = p->pitch * p->height;

    // 3. 内存映射
    p->fbmem = mmap(NULL, p->screensize, PROT_WRITE,
                    MAP_SHARED, p->lcdfd, 0);
    printf("[%s][%d]\n", __FILE__, __LINE__);
    if(p->fbmem == MAP_FAILED)
    {
        perror("mmap failed");
        return false;
    }

    // 清屏
    printf("[%s][%d]\n", __FILE__, __LINE__);
    bzero(p->fbmem, p->screensize);
    return true;
}

void lcd_release(struct lcdinfo *p)
{
    if(p == NULL)
        return;

    munmap(p->fbmem, p->screensize);
    close(p->lcdfd);
}