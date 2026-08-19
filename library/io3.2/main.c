#include <stdio.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include "bmp.h"
#include "jpg.h"
#include <stdlib.h>
bool show_jpg(char *argv)
{
    struct jpg_size *size = malloc(sizeof(struct jpg_size));
    //打开jpg文件
    int jpg_fd = open(argv, O_RDONLY);
    if(jpg_fd == -1)
    {
        printf("open file error\n");
        return 0;
    }
    //获取jpg文件大小
    int n = lseek(jpg_fd, 0, SEEK_END);
    int jpgsize = n;
    lseek(jpg_fd, 0, SEEK_SET);
    //读取jpg文件
    char *jpg = (char *)malloc(jpgsize);
    read(jpg_fd, jpg, jpgsize);
    //解码
    jpg2rgb(jpg, jpgsize, size);
    //关闭文件
    close(jpg_fd);
    //准备lcd
    int lcd_fd = open("/dev/fb0", O_RDWR);
    if(lcd_fd == -1)
    {
        printf("open lcd error\n");
        return 0;
    }
    //获取屏幕大小
    struct fb_var_screeninfo lcd_info;
    ioctl(lcd_fd, FBIOGET_VSCREENINFO, &lcd_info);
    struct lcd_size *lcd_size = (struct lcd_size *)malloc(sizeof(struct lcd_size));
    lcd_size->width = lcd_info.xres;
    lcd_size->height = lcd_info.yres;
    lcd_size->bpp = lcd_info.bits_per_pixel;
    //将jpg显示到lcd上
    char * p = mmap(NULL, lcd_size->width*lcd_size->height*lcd_size->bpp/8,
                 PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    bool show = showjpg(p, lcd_size, size, argv);
    free(jpg);
    free(size);
    free(lcd_size);
    close(lcd_fd);
    return true;
}
bool show_png(void)
{
    return true;
}
bool show_bmp(char * argv)
{
    show(argv);
    return true;

}
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("please input jpg file name\n");
        return 0;
    }
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1)
    {
        printf("open file error\n");
        return 0;
    }
    char * s = argv[1];
    if(strstr(argv[1], ".jpg") != NULL)
    {
        show_jpg(s);
    }
    else if(strstr(argv[1], ".bmp") != NULL)
    {
        show_bmp(s);
    }
    else if(strstr(argv[1], ".png") != NULL)
    {
        show_png();
    }
    free(s);
    close(fd);
    return 0;
}
