#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include "bmp.h"

// 显示指定的一张bmp格式的图片

int main(int argc, char const *argv[]) // ./a.out xxx.bmp
{
    if(argc != 2)
    {
        printf("usage: %s <bmp>\n", argv[0]);
        return -1;
    }

    // 准备LCD资源
    int lcd = open("/dev/fb0", O_RDWR);
    if(lcd == -1)
    {
        perror("open lcd failed");
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);

    int lcd_w = vinfo.xres;
    int lcd_h = vinfo.yres;
    int lcd_bpp = vinfo.bits_per_pixel;
    int lcd_line_size = lcd_w * lcd_bpp / 8;
    int lcd_size = lcd_w * lcd_h * lcd_bpp / 8;

    char *p = mmap(NULL, lcd_size, PROT_READ|PROT_WRITE, MAP_SHARED, lcd, 0);
    if(p == MAP_FAILED)
    {
        perror("mmap lcd failed");
        return -1;
    }

    // 读取bmp文件格式头
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1)
    {
        perror("open bmp failed");
        return -1;
    }

    // 读取bmp文件RGB数据
    struct bitmap_header header;
    struct bitmap_info   info;
    bzero(&header, sizeof(header));
    bzero(&info, sizeof(info));

    read(fd, &header, sizeof(header));
    read(fd, &info, sizeof(info));

    if(info.compression != 0)
    {
        struct rgb_quad quad;
        bzero(&quad, sizeof(quad));
        read(fd, &quad, sizeof(quad));
    }

    int bmp_w = info.width;
    int bmp_h = info.height;
    int bmp_bpp = info.bit_count;

    printf("图片分辨率:%d×%d\n", info.width, info.height);

    // 计算一行中的无效字节数
    int pad = (4 - (bmp_w*bmp_bpp/8 % 4)) % 4;
    int bmp_line_size = bmp_w * bmp_bpp / 8 + pad;
    int bmp_size = bmp_line_size * bmp_h;

    char *rgb = malloc(bmp_size);
    bzero(rgb, bmp_size);

    // 读取bmp文件RGB数据
    while(bmp_size > 0)
    {
        int n = read(fd, rgb, bmp_size); // n <= bmp_size
        if(n == -1)
        {
            perror("read bmp failed");
            return -1;
        }
        bmp_size -= n;
    }

    // 将RGB数据写入LCD
    int pixel = bmp_bpp/8;

    // 将整张BMP画上LCD
    for(int j=0; j<bmp_h && j<lcd_h; j++)
    {
        int lcd_offset = lcd_line_size*j;
        int bmp_offset = bmp_line_size*j;

        // 将图片的一行画上LCD
        for(int i=0; i<bmp_w && i<lcd_w; i++)
        {
            memcpy(p+4*i + lcd_offset,
                   rgb+i*pixel + bmp_offset, pixel);
        }
    }

    // 释放资源
    munmap(p, lcd_size);
    close(lcd);
    close(fd);
    free(rgb);

    return 0;
}