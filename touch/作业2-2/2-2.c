#include <stdio.h>
#include <linux/input.h>
#include "LCD.h"

enum IMG_FMT{BMP, JPG, PNG};

void show_jpg(struct lcdinfo *LCD, const char *filename);
void show_bmp(struct lcdinfo *LCD, const char *filename);
void show_png(struct lcdinfo *LCD, const char *filename);

void wait_press(int tp)
{
    struct input_event ev;
    bzero(&ev, sizeof(ev));

    while(1)
    {
        read(tp, &ev, sizeof(ev));

        // if(ev.type == EV_ABS && ev.code == ABS_PRESSURE && ev.value == 1)
        if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 1)
        {
            break;
        }
    }
}


void wait_release(int tp)
{
    struct input_event ev;
    bzero(&ev, sizeof(ev));

    while(1)
    {
        read(tp, &ev, sizeof(ev));

        // if(ev.type == EV_ABS && ev.code == ABS_PRESSURE && ev.value == 0)
        if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
        {
            break;
        }
    }
}

enum IMG_FMT get_fmt(const char *filename)
{
    if(strstr(filename, ".jpg") || strstr(filename, ".jpeg"))
        return JPG;
    
    if(strstr(filename, ".bmp"))
        return BMP;

    if(strstr(filename, ".png"))
        return PNG;
}


void show_image(struct lcdinfo *LCD, const char *filename)
{
    switch(get_fmt(filename))
    {
    case JPG:
        show_jpg(LCD, filename);
        break;
    
    case BMP:
        show_bmp(LCD, filename);
        break;

    case PNG:
        show_png(LCD, filename);
        break;
    }
}

int main(int argc, char const *argv[])
{
    // 1. 初始化LCD
    struct lcdinfo *LCD = calloc(1, sizeof(struct lcdinfo));
    lcd_init("/dev/fb0", LCD);
    show_image(LCD, "off.jpg");

    // 2. 初始化TP
    int tp = open("/dev/input/event6", O_RDONLY);
    if(tp < 0)
    {
        perror("open tp");
        return -1;
    }

    // 3. 实现触屏按按钮效果
    while(1)
    {
        wait_press(tp);
        printf("on.jpg\n");
        show_image(LCD, "on.jpg");

        wait_release(tp);
        printf("off.jpg\n");
        show_image(LCD, "off.jpg");
    }

    // 4. 释放资源
    release_lcd(LCD);
    
    return 0;
}
