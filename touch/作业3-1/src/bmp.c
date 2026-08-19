#include <stdio.h>
#include <stdbool.h>
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
#include <errno.h>

#include "lcd.h"
#include "image.h"

void show_image(struct lcdinfo *LCD, const char *bmpfile)
{
    printf("[%s][%d]\n", __FILE__, __LINE__);

    // 准备BMP图片资源
    int bmp_fd = open(bmpfile, O_RDWR);
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
    unsigned char *p = LCD->fbmem + LCD->pitch *(LCD->height-1); // 指向最后一行
    for(j=0; j<bmp_height && j<LCD->height; j++)
    {
        for(i=0; i<bmp_width && i<LCD->width ; i++)
        {
            read(bmp_fd, p+4*i, bmp_bpp/8);
        }

        // 跳过图片未显示的部分（可能包含有效图像，也可能仅包含pad部分）
        lseek(bmp_fd, bmp_pitch_real-bmp_bpp/8*i, SEEK_CUR);

        // 指向LCD显存的上一行
        p -= LCD->pitch;
    }
}
