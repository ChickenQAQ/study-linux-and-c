#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>        // open()  O_RDWR
#include <sys/ioctl.h>    // ioctl()
#include <linux/fb.h>     // struct fb_var_screeninfo  FBIOGET_VSCREENINFO
#include <sys/mman.h>     // mmap munmap PROT_* MAP_* MAP_FAILED
#include <unistd.h>       // close()
#include <jpeglib.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/input.h>   // struct input_event、EV_KEY、BTN_TOUCH 全部在这里
#include <linux/kd.h>
#include <linux/fb.h>
#include <sys/time.h>
#include "jpeglib.h"


int jpg_w, jpg_h;
 int x = 0, y = 0; //触摸点坐标
bool pressed = false; //当前手指是否按下

// 参数说明：
//   jpgdata: jpg图片数据
//   jpgsize: jpg图片大小
// 返回值说明：
//   成功：指向rgb数据的指针
//   失败：NULL
char *jpg2rgb(const char *jpgdata, size_t jpgsize, int *jpg_w, int *jpg_h)
{
    // 1，声明解码结构体，以及错误管理结构体
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // 2，使用缺省的出错处理来初始化解码结构体
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // 3，配置该cinfo，使其从 jpgdata 中读取jpgsize个字节
    //    这些数据必须是完整的JPEG数据
    jpeg_mem_src(&cinfo, jpgdata, jpgsize);

    // 4，读取JPEG文件的头，并判断其格式是否合法
    if(!jpeg_read_header(&cinfo, true))
    {
        fprintf(stderr, "jpeg_read_header failed: "
            "%s\n", strerror(errno));
        return NULL;
    }
    cinfo.out_color_space = JCS_RGB;
    // 5，开始解码
    jpeg_start_decompress(&cinfo);

    // 6，获取图片的尺寸信息
    *jpg_w = cinfo.output_width;
    *jpg_h = cinfo.output_height;
    //printf("宽：  %d\n", cinfo.output_width);
    //printf("高：  %d\n", cinfo.output_height);
    //printf("色深：%d\n", cinfo.output_components);

    int row_stride = cinfo.output_width * cinfo.output_components;

    // 7，根据图片的尺寸大小，分配一块相应的内存rgbdata
    //    用来存放从jpgdata解码出来的图像数据
    unsigned long linesize = cinfo.output_width * cinfo.num_components;
    unsigned long rgbsize  = linesize * cinfo.output_height;
    char *rgbdata = calloc(1, rgbsize);

    // 8，循环地将图片的每一行读出并解码到rgb_buffer中
    int line = 0;
    while(cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = rgbdata + cinfo.output_scanline * linesize;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    // 9，解码完了，将jpeg相关的资源释放掉
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return rgbdata;
}

bool showjpg(char *p, int fb_w, int fb_h, int fb_bpp,char * jpg)
{
    //准备jpg1
    FILE *fp = fopen(jpg, "rb");
    if(fp == NULL)
    {
        fprintf(stderr, "open %s failed: %s\n", jpg, strerror(errno));
        return false;
    }
    fseek(fp, 0, SEEK_END);
    long jpgsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *jpgdata = malloc(jpgsize);
    fread(jpgdata, 1, jpgsize, fp);
    fclose(fp);

    //将jpg转换为rgb
    char *rgbdata = jpg2rgb(jpgdata, jpgsize, &jpg_w, &jpg_h);
    if(rgbdata == NULL)
    {
        fprintf(stderr, "jpg2rgb failed\n");
        return false;
    }

    //将rgb显示到lcd上
    int rgb_w = jpg_w;
    int rgb_h = jpg_h;
    int rgb_bpp = 3;
    int rgb_size = rgb_w * rgb_h * rgb_bpp;
    int fb_byte = fb_bpp / 8;   // 位深转字节数，32位深就是4字节
    int x = (fb_w - rgb_w) / 2;
    if(x<0) x = 0;
    int y = (fb_h - rgb_h) / 2;
    if(y<0) y = 0;
    for(int i = 0; i < rgb_h&&i<fb_h; i++)
    {
        int lcd_offset = (y+i)*fb_w*fb_byte + x*fb_byte;
        int rgb_offset = i*rgb_w*rgb_bpp;
        for(int j = 0; j < rgb_w&&j<fb_w; j++)
        {
            char *lcd = p + lcd_offset + j*fb_byte;
            memcpy(lcd, rgbdata + rgb_offset + j*rgb_bpp, rgb_bpp);
            //将RGB转换为BGR
            char tmp = lcd[0];
            lcd[0] = lcd[2];
            lcd[2] = tmp;
        }
    }
    free(jpgdata);
    free(rgbdata);
    return true;
}
bool istouch (struct input_event * ev,int fb_w,int fb_h)
{
    // 1，坐标事件：只更新坐标，不做判断
    if(ev->type == EV_ABS && ev->code == ABS_X)
    {
        x = ev->value;
        return false;
    }
    if(ev->type == EV_ABS && ev->code == ABS_Y)
    {
        y = ev->value;
        return false;
    }
    // 2，按键事件：只记录按下/抬起状态
    if(ev->type == EV_KEY && ev->code == BTN_TOUCH)
    {
        pressed = (ev->value == 1);
        return false;
    }
    // 3，帧结束事件：一帧的坐标都到齐了，这时判断才准
    if(ev->type == EV_SYN && ev->code == SYN_REPORT)
    {
        if(pressed
            && x > (fb_w-jpg_w)/2 && x < jpg_w+(fb_w-jpg_w)/2
            && y > (fb_h-jpg_h)/2 && y < jpg_h+(fb_h-jpg_h)/2)
        {
            printf("点击了按钮(x=%d,y=%d)\n", x, y);
            return true;
        }
    }
    return false;
}
int main(int argc , char **argv)
{
    //准备lcd
    int lcd = open("/dev/fb0", O_RDWR);
    if(lcd < 0)
    {
        fprintf(stderr, "open /dev/fb0 failed: %s\n", strerror(errno));
        return -1;
    }
    static struct fb_var_screeninfo fb_var;
    ioctl(lcd, FBIOGET_VSCREENINFO, &fb_var);
    int fb_w = fb_var.xres;
    int fb_h = fb_var.yres;
    int fb_bpp = fb_var.bits_per_pixel;
    int fb_size = fb_w * fb_h * fb_bpp / 8;

    char *p = mmap(NULL, fb_size, PROT_READ | PROT_WRITE
                    , MAP_SHARED, lcd, 0);
    if(p == MAP_FAILED)
    {
        fprintf(stderr, "mmap failed: %s\n", strerror(errno));
        return -1;
    }


    //清屏
    memset(p, 0, fb_size);
    showjpg(p, fb_w, fb_h, fb_bpp, "off.jpg");
    

    //准备触摸屏
    int touch = open("/dev/input/event6", O_RDONLY);
    if(touch < 0)
    {
        fprintf(stderr, "open /dev/input/event6 failed: %s\n", strerror(errno));
        return -1;
    }

    while(1)
    {
        struct input_event ev;
        read(touch, &ev, sizeof(ev));
        if(istouch(&ev, fb_w, fb_h))
        {
            showjpg(p, fb_w, fb_h, fb_bpp, "on.jpg");
            sleep(1);
        }
        showjpg(p, fb_w, fb_h, fb_bpp, "off.jpg");
    }
    munmap(p, fb_size);
    close(lcd);
    close(touch);

    return 0;
}