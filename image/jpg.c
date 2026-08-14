
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
#include "jpeglib.h"

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
    *jpg_w = cinfo.output_width;
    *jpg_h = cinfo.output_height;

    // 6，获取图片的尺寸信息
    printf("宽：  %d\n", cinfo.output_width);
    printf("高：  %d\n", cinfo.output_height);
    printf("色深：%d\n", cinfo.output_components);

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
int main(int argc,char **argv)
{
    if(argc != 2)
    {
        printf("Usage: %s <jpgfile>\n", argv[0]);
        return -1;
    }
    //部署lcd
    int lcd = open("/dev/fb0", O_RDWR);
    if(lcd < 0)
    {
        perror("open /dev/fb0 failed");
        return -1;
    }
    struct fb_var_screeninfo  fb_var;
    if(ioctl(lcd, FBIOGET_VSCREENINFO, &fb_var) < 0)
    {
        perror("ioctl FBIOGET_VSCREENINFO failed");
        return -1;
    }
    int lcd_w=fb_var.xres;
    int lcd_h=fb_var.yres;
    int lcd_bpp=fb_var.bits_per_pixel;
    int lcd_line = lcd_w*lcd_bpp/8;
    int lcd_size=lcd_w*lcd_h*lcd_bpp/8;
    
    char *p =mmap(NULL, lcd_size, PROT_READ | PROT_WRITE,
                 MAP_SHARED, lcd, 0);
    if(p == MAP_FAILED)
    {
        perror("mmap failed");
        return -1;
    }
    
    
    //打开jpg文件
    FILE *fp = fopen(argv[1], "rb");
    if(!fp)
    {
        printf("fopen %s failed: %s\n", argv[1], strerror(errno));
        return -1;
    }
    //获取jpg文件大小
    fseek(fp, 0, SEEK_END);
    size_t jpgsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *jpgdata = malloc(jpgsize);
    fread(jpgdata, 1, jpgsize, fp);
    int jpg_w = 0, jpg_h = 0;
    char *n = jpg2rgb(jpgdata, jpgsize, &jpg_w, &jpg_h);
    if(n==NULL)
    {
        printf("jpg2rgb failed\n");
        return -1;
    }
    printf("jpg: %dx%d\n", jpg_w, jpg_h);

    memset(p, 0, lcd_size);              // 先清屏，好判断到底画没画上
    //将照片显示到lcd上（jpg每像素3字节，屏每像素 lcd_bpp/8 字节）
    //将照片显示在中心处
    int x = (lcd_w-jpg_w)/2;
    if(x<0) x=0;
    int y = (lcd_h-jpg_h)/2;
    if(y<0) y=0;
    for(int i=0;i<lcd_h&&i<jpg_h;i++)
    {
        int lcd_offset = lcd_line*(y+i);
        int jpg_offset = jpg_w*3*i;      // 图片每行字节数
        for(int j=0;j<lcd_w&&j<jpg_w;j++)
        {
            char *q = p+lcd_offset+(j+x)*lcd_bpp/8;   // 屏上的位置
            memcpy(q, n+jpg_offset+j*3, 3);       // 拷3字节
            //将RGB转为BGR，就是把q[0]和q[2]交换
            char t = q[0];                        // 红蓝对调
            q[0] = q[2];
            q[2] = t;
        }
    }

    munmap(p, lcd_size);
    close(lcd);
    free(jpgdata);
    return 0;
}