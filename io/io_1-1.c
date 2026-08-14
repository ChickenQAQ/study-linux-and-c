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
//在开发板中循环输出八个色块
int main()
{
    int lcd = open("/dev/fb0", O_RDWR);
    if(lcd == -1)
    {
        perror("open lcd failed");
        exit(1);
    }
    struct fb_var_screeninfo vinfo;
    ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);
    int width = vinfo.xres;//获取屏幕的高
    int height = vinfo.yres;//获取屏幕的宽
    int bpp = vinfo.bits_per_pixel;//获取屏幕的像素点
    int size = width * height * bpp / 8;//获取屏幕的像素点大小
    int32_t *fb = mmap(NULL, size, PROT_READ | PROT_WRITE,
                                      MAP_SHARED, lcd, 0);
    if(fb == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }
    uint32_t colors [8] = {
    0x00FF0000,  // 红
    0x0000FF00,  // 绿
    0x000000FF,  // 蓝
    0x00FFFF00,  // 黄
    0x00FF00FF,  // 品红(洋红)
    0x0000FFFF,  // 青色
    0x00FF8800,  // 橙
    0x008844FF   // 紫
    };
    while(1)
    {
        for(int i = 0;; i++)
        {
            for(int j = 0; j < height/2; j++)
            {
                for(int k = 0; k<width/4; k++)
                {
                    fb[j*width + k] = colors[(i+0)%8];
                }
                for(int k = width/4; k<width/2; k++)
                {
                    fb[j*width + k] = colors[(i+1)%8];
                }
                for(int k = width/2; k<width*3/4; k++)
                {
                    fb[j*width + k] = colors[(i+2)%8];
                }
                for(int k = width*3/4; k<width; k++)
                {
                    fb[j*width + k] = colors[(i+3)%8];
                }
            }
            for(int j = height/2; j < height; j++)
            {
                for(int k = 0; k<width/4; k++)
                {
                    fb[j*width + k] = colors[(i+7)%8];
                }
                for(int k = width/4; k<width/2; k++)
                {
                    fb[j*width + k] = colors[(i+6)%8];
                }
                for(int k = width/2; k<width*3/4; k++)
                {
                    fb[j*width + k] = colors[(i+5)%8];
                }
                for(int k = width*3/4; k<width; k++)
                {
                    fb[j*width + k] = colors[(i+4)%8];
                }
            }
            sleep(1);
        }
    }
}