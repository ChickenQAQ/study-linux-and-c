#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdint.h>
//在开发板中循环显示三色国旗
int main()
{
    int lcd = open("/dev/fb0", O_RDWR);
    if (lcd == -1)
    {
        perror("open");
        return -1;
    }
    struct fb_var_screeninfo vinfo;
    ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);
    int width = vinfo.xres;
    int height = vinfo.yres; //获取屏幕的宽高
    int bpp = vinfo.bits_per_pixel; //获取屏幕的每个像素的位数
    int size = width * height * bpp / 8; //计算屏幕大小
    printf("屏幕分辨率为%d*%d\n",width,height);
    int32_t *p = mmap(NULL, size, PROT_READ | PROT_WRITE, 
                                    MAP_SHARED, lcd, 0);
    if (p == MAP_FAILED)
    {
        perror("mmap failed");
        return -1;
    }
    uint32_t colors[] = {0xff0000, 0x00ff00, 0x0000ff};
    uint32_t france[] = {0x0000ff,0xffffff,0xff0000};
    uint32_t russia[] = {0xffffff,0x0000ff,0xff0000};
    uint32_t germany[] = {0x000000,0xff0000,0xffff00};
    while(1)
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                p[i * width + j] = france[j / (width / 3)];
            }
        }
        sleep(1);
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                p[i * width + j] = russia[i / (height / 3)];
            }
        }
        sleep(1);
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                p[i * width + j] = germany[i / (height / 3)];
            }
        }
        sleep(1);
    }
    
    munmap(p, size);
    close(lcd);
    return 0;
}