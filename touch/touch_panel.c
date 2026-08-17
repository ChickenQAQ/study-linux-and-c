#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>


// 读取触摸屏的轨迹坐标，并以(x,y)连续输出

int main(int argc, char const *argv[])
{
    // 1. 打开触摸屏设备
    int tp = open("/dev/input/event6", O_RDONLY);
    if(tp < 0)
    {
        perror("打开/dev/input/event6失败");
        exit(1);
    }

    // 2. 循环读取触摸屏数据
    struct input_event ev;
    while(1)
    {
        bzero(&ev, sizeof(ev));

        int n = read(tp, &ev, sizeof(ev));
        if(n < 0)
        {
            perror("读取触摸屏数据失败");
            exit(1);
        }

        if(ev.type == EV_ABS // 读取到绝对坐标事件类型
        && ev.code == ABS_X) // 读取到x坐标
        {
            printf("(%d,", ev.value); // (x,
        }
        if(ev.type == EV_ABS // 读取到绝对坐标事件类型
        && ev.code == ABS_Y) // 读取到y坐标
        {
            printf("%d)\n", ev.value); // y)
        }

        if(ev.type == EV_KEY     // 读取到按键事件类型
        && ev.code == BTN_TOUCH) // 读取到触摸事件
        {
            if(ev.value = 1)       // 按下了触摸屏
                printf("按下触摸屏\n");
            else
                printf("松开触摸屏\n");
        }

        // if(ev.type == EV_ABS        // 读取到绝对坐标事件类型
        // && ev.code == ABS_PRESSURE) // 读取到压力值
        // {
        //     if(ev.value == 1)       // 按下了触摸屏
        //         printf("按下触摸屏\n");
        //     else
        //         printf("松开触摸屏\n");
        // }
    }

    return 0;
}
