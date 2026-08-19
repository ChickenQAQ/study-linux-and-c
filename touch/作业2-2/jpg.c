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

#include "jpeglib.h"
#include "LCD.h"

int jpg_w;
int jpg_h;
int jpg_bpp; // 色深（单位是比特）
int jpg_pitch;

// 参数说明：
//   jpgdata: jpg图片数据
//   jpgsize: jpg图片大小
// 返回值说明：
//   成功：指向rgb数据的指针
//   失败：NULL
char *jpg2rgb(const char *jpgdata, size_t jpgsize)
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

    // 5，开始解码
    jpeg_start_decompress(&cinfo);

    // 6，获取图片的尺寸信息
    printf("宽：  %d\n", jpg_w = cinfo.output_width);
    printf("高：  %d\n", jpg_h = cinfo.output_height);
    printf("色深：%d\n", jpg_bpp = cinfo.output_components*8);
    jpg_pitch = cinfo.output_width * cinfo.output_components;

    // 7，根据图片的尺寸大小，分配一块相应的内存rgbdata
    //    用来存放从jpgdata解码出来的图像数据
    unsigned long linesize = cinfo.output_width * cinfo.output_components;
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

void show_jpg(struct lcdinfo *LCD, const char *filename)
{
    // 0. 获取文件数据
    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "fopen failed: %s\n", strerror(errno));
        return;
    }

    fseek(fp, 0, SEEK_END);
    long jpgsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *jpgdata = calloc(1, jpgsize);

    // 1. 解码JPG
    char *rgbdata = jpg2rgb(jpgdata, jpgsize);

    //缩放图片
    float scale_w =(float) LCD->height / jpg_h;
    float scale_h = (float)LCD->width  / jpg_w;
    float scale = scale_w < scale_h ? scale_w : scale_h;
    int new_w = (int)(jpg_w * scale);
    int new_h = (int)(jpg_h * scale);
    int office_x = (LCD->width - new_w) / 2;
    int office_y = (LCD->height - new_h) / 2;

    // 2. 显示RGB
    for(int j=0; j<new_h && j<LCD->height; j++)
    {
        for(int i=0; i<new_w && i<LCD->width; i++)
        {
            //屏幕的坐标
            int out_x = i + office_x;
            int out_y = j + office_y;
            //图片的坐标
            int x = (int)(i / scale);
            int y = (int)(j / scale);
            
        }
    }

    // 3. 释放资源
    free(jpgdata);
    free(rgbdata);
    fclose(fp);
}