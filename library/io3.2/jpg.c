#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <stdbool.h>
#include <linux/fb.h>

#include <jpeglib.h>
#include <jerror.h>

#include "jpg.h"

char *jpg2rgb(const char *jpgdata, size_t jpgsize, struct jpg_size *jpginfo)
    {
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        jpeg_mem_src(&cinfo, jpgdata, jpgsize);

        if(!jpeg_read_header(&cinfo, true))
        {
            fprintf(stderr, "jpeg_read_header failed: %s\n", strerror(errno));
            return NULL;
        }
        cinfo.out_color_space = JCS_RGB;
        jpeg_start_decompress(&cinfo);

        jpginfo->width = cinfo.output_width;
        jpginfo->height = cinfo.output_height;

        int linesize = cinfo.output_width * cinfo.num_components;
        unsigned long rgbsize  = (unsigned long)linesize * cinfo.output_height;
        char *rgbdata = calloc(1, rgbsize);

        while(cinfo.output_scanline < cinfo.output_height)
        {
            unsigned char *buffer_array[1];
            buffer_array[0] = (unsigned char *)(rgbdata + cinfo.output_scanline * linesize);
            jpeg_read_scanlines(&cinfo, buffer_array, 1);
        }

        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        return rgbdata;
    }

bool showjpg(char *p, struct lcd_size *lcdinfo, struct jpg_size *jpginfo, char *jpg)
{
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

    char *rgbdata = jpg2rgb(jpgdata, jpgsize, jpginfo);
    if(rgbdata == NULL)
    {
        fprintf(stderr, "jpg2rgb failed\n");
        free(jpgdata);
        return false;
    }

    int fb_w = lcdinfo->width;
    int fb_h = lcdinfo->height;
    int fb_bpp = lcdinfo->bpp;
    int fb_byte = fb_bpp / 8;

    int rgb_w = jpginfo->width;
    int rgb_h = jpginfo->height;
    const int rgb_bpp = 3;

    //缩放图片
    float scale_x = (float)fb_w / rgb_w;
    float scale_y = (float)fb_h / rgb_h;
    float scale = scale_x < scale_y ? scale_x : scale_y;


    int draw_x = rgb_w * scale;
    int draw_y = rgb_h * scale;
    int offset_x = (fb_w - draw_x) / 2;
    int offset_y = (fb_h - draw_y) / 2;
    //写入前先清屏
    memset(p, 0, fb_w * fb_h * fb_byte);

    for(int i = 0; i < draw_y; i++)
    {
        
        for(int j = 0; j < draw_x; j++)
        {
            //屏幕的偏移
            int out_x = offset_x + j;
            int out_y = offset_y + i;
            //图像的偏移
            int src_x = j / scale;
            int src_y = i / scale;
            memcpy(p + (out_y * fb_w + out_x) * fb_byte,
             rgbdata + (src_y * rgb_w + src_x) * rgb_bpp, rgb_bpp);
            //rgb转成brg
            char tmp = p[(out_y * fb_w + out_x) * fb_byte];
            p[(out_y * fb_w + out_x) * fb_byte] = p[(out_y * fb_w + out_x) * fb_byte + 2];
            p[(out_y * fb_w + out_x) * fb_byte + 2] = tmp;
        }
    }
    free(jpgdata);
    free(rgbdata);
    return true;
}