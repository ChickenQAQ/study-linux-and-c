#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <time.h>
#include <stdbool.h>

int fb_w, fb_h; // 画布宽高，main里查一次，全局共享

// 画一个点（带越界保护，防止手指划出屏幕时写坏内存）
void put_pixel(int *p, int x, int y, int color)
{
    if(x < 0 || x >= fb_w || y < 0 || y >= fb_h)
        return;
    p[x + y * fb_w] = color;
}

// 画线：从(x0,y0)连到(x1,y1)，保证快速滑动不断线
// 原理：算出两点间需要走多少步，每一步前进一格像素
void draw_line(int *p, int x0, int y0, int x1, int y1, int color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if(steps == 0)
    {
        put_pixel(p, x0, y0, color);
        return;
    }
    float incx = (float)dx / steps;
    float incy = (float)dy / steps;
    float px = x0, py = y0;
    for(int i = 0; i <= steps; i++)
    {
        put_pixel(p, (int)px, (int)py, color);
        px += incx;
        py += incy;
    }
}

int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));

    // LCD
    int lcd = open("/dev/fb0", O_RDWR);
    if(lcd < 0)
    {
        perror("open /dev/fb0");
        return -1;
    }
    struct fb_var_screeninfo vinfo;
    ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);
    fb_w = vinfo.xres;
    fb_h = vinfo.yres;

    int *p = (int *)mmap(NULL, fb_w * fb_h * 4,
                         PROT_READ | PROT_WRITE, MAP_SHARED, lcd, 0);
    if(p == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }

    // 白色画布（memset按字节填，每字节0xff就是白色）
    memset(p, 0xff, fb_w * fb_h * 4);

    // 触摸
    int fd = open("/dev/input/event6", O_RDONLY);
    if(fd < 0)
    {
        perror("open /dev/input/event6");
        return -1;
    }

    int color = 0;          // 当前画笔颜色
    int x = 0, y = 0;       // 当前坐标
    int last_x = 0, last_y = 0; // 上一个点，用于连线
    bool pressed = false;      // 手指是否按住
    bool prev_pressed = false; // 上一帧是否按住

    struct input_event ev;
    while(1)
    {
        if(read(fd, &ev, sizeof(ev)) != sizeof(ev))
            continue;

        if(ev.type == EV_ABS && ev.code == ABS_X)
            x = ev.value;
        else if(ev.type == EV_ABS && ev.code == ABS_Y)
            y = ev.value;
        else if(ev.type == EV_KEY && ev.code == BTN_TOUCH)
            pressed = (ev.value == 1);
        else if(ev.type == EV_SYN && ev.code == SYN_REPORT)
        {
            // 帧结束，坐标齐了，判断要不要画
            if(pressed && !prev_pressed)
            {
                // 新一笔：换随机颜色，落起点
                color = rand() % 0xffffff;
                put_pixel(p, x, y, color);
                last_x = x;
                last_y = y;
            }
            else if(pressed)
            {
                // 按住拖动：从上一个点连到当前点
                draw_line(p, last_x, last_y, x, y, color);
                last_x = x;
                last_y = y;
            }
            prev_pressed = pressed;
        }
    }

    munmap(p, fb_w * fb_h * 4);
    close(lcd);
    close(fd);
    return 0;
}
