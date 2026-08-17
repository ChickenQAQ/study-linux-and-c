#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>      // 提供 mmap(), PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED
#include <linux/fb.h>      // 提供 FBIOGET_VSCREENINFO, struct fb_var_screeninfo
#include <sys/ioctl.h>     // 提供 ioctl()
#include <time.h>          // 提供 rand() 如果需要
#include <linux/input.h>   // 提供 EV_KEY, BTN_TOUCH, EV_ABS, ABS_X, ABS_Y, EV_SYN, SYN_REPORT
#include <stdbool.h>
#include <string.h>
bool istouch (struct input_event ev)
{
    if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 1)
        {
            return true;
        }
    return false;
}

int main(int argc, char *argv[])
{


    //lcd部分
    int lcd = open("/dev/fb0", O_RDWR);
    if (lcd < 0) {
        printf("open /dev/fb0 failed\n");
        return -1;
    }
    //设置屏幕大小
    struct fb_var_screeninfo vinfo;
    ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);
    int lcd_width = vinfo.xres;
    int lcd_height = vinfo.yres;
    int lcd_size = lcd_width * lcd_height * 4;
    int lcd_bpp = vinfo.bits_per_pixel;
    int lcd_line=lcd_width * lcd_bpp / 8;
    
    //初始化屏幕
    int  *p = mmap(NULL, lcd_size, PROT_READ | PROT_WRITE
                , MAP_SHARED, lcd, 0);
    if (p == MAP_FAILED) {
        printf("mmap /dev/fb0 failed\n");
        return -1;
    }
    memset(p, 0, lcd_size);

    int color = 0x00ff0000;
    //触摸屏部分
    int tp = open("/dev/input/event6", O_RDONLY);
    if (tp < 0) {
        printf("open /dev/input/event6 failed\n");
        return -1;
    }
    struct input_event ev;
    int x , y;
    while(1)
    {
        int n = read(tp, &ev, sizeof(struct input_event));
        if(n<0)
        {
            printf("read /dev/input/event6 failed\n");
            return -1;
        }
        if(istouch(ev))
        {
            //随机输出颜色
            color = rand() % 0xffffff;
            for(int i = 0 ;i< lcd_height*lcd_width ; i++)
            {
                p[i] = color;
            }
        }
    }
    close(tp);
    return 0;
}