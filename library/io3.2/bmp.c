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
#include <stdbool.h>

#include "bmp.h"

// 显示指定的一张bmp格式的图片

bool show(char *argv)
{
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
    int fd = open(argv, O_RDONLY);
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
    int read_len = bmp_size;
    int read_off = 0;
    while(read_len > 0)
    {
        int n = read(fd, rgb + read_off, read_len); // n <= bmp_size
        if(n == -1)
        {
            perror("read bmp failed");
            return false;
        }
        read_len -= n;
        read_off += n;
    }
    //写入前先清屏
    memset(p, 0, lcd_size);

    // 将RGB数据写入LCD
    // BMP和LCD的每像素字节数可能不同, 必须分开算
    int bmp_pixel = bmp_bpp/8; // BMP每像素字节数 (24bit -> 3)
    int lcd_pixel = lcd_bpp/8; // LCD每像素字节数 (16->2, 24->3, 32->4)

    //缩放图片
    float scale_x = (float)lcd_w / bmp_w;
    float scale_y = (float)lcd_h / bmp_h;
    float scale = scale_x < scale_y ? scale_x : scale_y;

    int draw_x = bmp_w*scale;
    int draw_y = bmp_h*scale;
    int start_x = (lcd_w - draw_x) / 2;
    int start_y = (lcd_h - draw_y) / 2;
    // 将整张BMP画上LCD
    for(int j=0; j<draw_y ; j++)
    {
        // 将图片的一行画上LCD
        for(int i=0; i<draw_x ; i++)
        {
            //屏幕的偏移
            int x = start_x + i;
            int y = start_y + j;
            //图片的偏移
            int x1 = i/scale;
            int y1 = bmp_h - 1 - (j/scale);
            if(x1 >= bmp_w) x1 = bmp_w -1;
            if(y1 >= bmp_h) y1 = bmp_h -1;
            // 计算RGB数据偏移
            int offset = y1 * bmp_line_size + x1 * bmp_pixel;

            char *src = rgb + offset;              // BMP数据: BGR(BMP天生BGR存储)
            char *dst = p + y * lcd_line_size + x * lcd_pixel;

            if(lcd_bpp == 24)
            {
                // LCD也是24bit BGR, 直接拷
                memcpy(dst, src, 3);
            }
            else if(lcd_bpp == 32)
            {
                // 32bit: BGR + alpha位(内存序 BB GG RR 00)
                memcpy(dst, src, 3);
                dst[3] = 0x00; // 若偏色/花屏, 把0x00改成0xFF试试
            }
            else if(lcd_bpp == 16)
            {
                // 16bit RGB565: 5bit红 + 6bit绿 + 5bit蓝
                unsigned char b = src[0], g = src[1], r = src[2];
                unsigned short color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                *(unsigned short *)dst = color;
            }
            else
            {
                printf("不支持的LCD位深:%d\n", lcd_bpp);
                munmap(p, lcd_size);
                close(lcd);
                close(fd);
                free(rgb);
                return false;
            }
        }
    }

    // 释放资源
    munmap(p, lcd_size);
    close(lcd);
    close(fd);
    free(rgb);

    return true;
}