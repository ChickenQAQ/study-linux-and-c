#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "bmp.h"

// void f(void)
// {
//     return; // 不退出程序
//     exit(); // 直接退出程序
// }

// 在LCD屏幕上显示一张指定的BMP图片

int main(int argc, char const *argv[])
{
    // 判定参数合法性
    if (argc != 2)
    {
        printf("Usage: %s <bmp_file>\n", argv[0]);
        exit(0);
    }

    // 准备lcd资源
    int lcd_fd = open("/dev/fb0", O_RDWR);
    if (lcd_fd == -1)
    {
        perror("open lcd failed");
        exit(1);
    }

    // 获取屏幕参数
    struct fb_var_screeninfo lcd_info;
    ioctl(lcd_fd, FBIOGET_VSCREENINFO, &lcd_info);
    int lcd_width = lcd_info.xres;
    int lcd_height = lcd_info.yres;
    int lcd_bpp = lcd_info.bits_per_pixel;

    // 获得LCD的映射内存
    unsigned char *lcd_mem = mmap(NULL, lcd_width * lcd_height * lcd_bpp / 8,
                                  PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (lcd_mem == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }

    memset(lcd_mem, 0xFF, lcd_width * lcd_height * lcd_bpp / 8);

    // 准备BMP图片资源
    int bmp_fd = open(argv[1], O_RDWR);
    if (bmp_fd == -1)
    {
        perror("open bmp failed");
        exit(1);
    }

    // 获取BMP图片参数
    struct bitmap_header bmp_header;
    struct bitmap_info   bmp_info;
    bzero(&bmp_header, sizeof(bmp_header));
    bzero(&bmp_info, sizeof(bmp_info));
    read(bmp_fd, &bmp_header, sizeof(bmp_header));
    read(bmp_fd, &bmp_info, sizeof(bmp_info));

    if(bmp_info.compression != 0)
    {
        struct rgb_quad quad;
        bzero(&quad, sizeof(quad));
        read(bmp_fd, &quad, sizeof(quad));
    }

    int bmp_width = bmp_info.width;
    int bmp_height = bmp_info.height;
    int bmp_bpp = bmp_info.bit_count;


    //                 65     *  3 = 195 + pad = 196
    int bmp_pitch = bmp_width * bmp_bpp / 8; // 不含无效字节pad
    int pad = (4-bmp_pitch%4) % 4;
    int bmp_pitch_real = bmp_pitch + pad;

    int bmp_size = bmp_pitch * bmp_height;   // 不含无效字节pad
    int bmp_size_real = bmp_pitch_real * bmp_height;

    // 直接将bmp剩下的RGB数据读取到LCD的映射内存中
    int i, j;
    unsigned char *p = lcd_mem;
    for(j=0; j<bmp_height && j<lcd_height; j++)
    {
        for(i=0; i<lcd_width && i<bmp_width; i++)
        {
            int got = 0;
            while (got < bmp_bpp / 8) 
            {
                ssize_t r = read(bmp_fd, p + 4 * i + got, bmp_bpp / 8 - got);
                if (r <= 0) 
                {
                    perror("read bmp failed");
                    exit(1);
                }
                got += r;
            }
    }

        }

        // 跳过图片未显示的部分（可能包含有效图像，也可能仅包含pad部分）
        lseek(bmp_fd, bmp_pitch_real-bmp_bpp/8*i, SEEK_CUR);

        // 指向LCD显存的下一行
        p += lcd_width * lcd_bpp / 8;
    }

    // unsigned char *p = lcd_mem;
    // while(bmp_size > 0)
    // {
    //     int n = read(bmp_fd, p, bmp_size_real);

    //     if( n == -1 )
    //     {
    //         perror("read bmp failed");
    //         exit(1);
    //     }

    //     if( n == 0 )
    //     {
    //         break;
    //     }

    //     bmp_size -= n;
    //     p += n;
    // }

    // 释放资源
    munmap(lcd_mem, lcd_width * lcd_height * lcd_bpp / 8);
    close(lcd_fd);
    close(bmp_fd);

    return 0;
}
