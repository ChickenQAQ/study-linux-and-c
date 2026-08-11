#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

int main()
{
    int lcd = open("/dev/fb0", O_RDWR);
    if(lcd < 0)
    {
        perror("open fb0");
        return -1;
    }

    // 【RK3568重点】ioctl读取屏幕硬件信息，不能写死分辨率
    struct fb_var_screeninfo var;
    ioctl(lcd, FBIOGET_VSCREENINFO, &var);

    int xres = var.xres;        // 屏幕宽
    int yres = var.yres;        // 屏幕高
    int bpp  = var.bits_per_pixel; // 每个像素字节位数，一般32(RGBA8888)

    int screen_size = xres * yres * (bpp / 8);

    // mmap映射显存
    unsigned int *p = mmap(NULL, screen_size,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED, lcd, 0);
    if(p == MAP_FAILED)
    {
        perror("mmap");
        close(lcd);
        return -1;
    }

    // 填充全屏红色，RK3568多数为BGRA格式
    unsigned int red = 0xFF0000FF;
    for(int i = 0; i < xres*yres; i++)
    {
        p[i] = red;   // 直接赋值，不用memcpy
    }

    sleep(3);

    // 释放资源
    munmap(p, screen_size);
    close(lcd);
    return 0;
}