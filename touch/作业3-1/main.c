#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "image.h"
#include "lcd.h"

enum IMG_FMT{BMP, JPG, PNG};

enum IMG_FMT get_fmt(const char *filename)
{
    if(strstr(filename, ".jpg") || strstr(filename, ".jpeg"))
        return JPG;
    
    if(strstr(filename, ".bmp"))
        return BMP;

    if(strstr(filename, ".png"))
        return PNG;
}

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s <image>\n", argv[0]);
        return -1;
    }

    // 1. 初始化LCD
    struct lcdinfo *LCD = calloc(1, sizeof(struct lcdinfo));
    lcd_init("/dev/fb0", LCD);

    // 2. 根据具体格式图片，选择不同的库函数来显示
    void *handler = NULL;
    void (*show_image)(struct lcdinfo *, const char *) = NULL;

    switch(get_fmt(argv[1]))
    {
    case JPG: 
        handler = dlopen("libjpg.so", RTLD_LAZY);
        break;
    case BMP:
        handler = dlopen("libbmp.so", RTLD_LAZY);
        break;
    // case PNG:
    //     handler = dlopen("libpng.so", RTLD_LAZY);
    //     break;
    }
    show_image = dlsym(handler, "show_image");

    show_image(LCD, argv[1]);

    // 3. 释放资源
    lcd_release(LCD);    return 0;
}
