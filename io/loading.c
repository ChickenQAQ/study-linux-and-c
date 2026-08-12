#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    //验证行缓冲和不缓冲
    fprintf(stdout, "%s", "hello world");//行缓冲，会比不缓冲慢,但是若是遇到\n会立即输出
    fprintf(stderr, "%s", "HELLO WORLD\n");//不缓冲，直接输出
    //sleep(1);
    //先输出HELLO WORLD,后输出hello world
    //验证全缓冲和不缓冲
    FILE *fp = fopen("chicken1.txt", "w");
    int i;
    char message[]= "0123456789";
    for(i=0;i<15000;i++)
    {
        fputc(message[i%10], fp);
    } 
    pause();//暂停，等待信号
    //fputs("appletree",fp);
    //fflush(fp);//强制刷新
    //return 0;
    //_exit(0);
    //abort();是一种给自己发信号，异常退出，会导致数据无法刷新到文件中，所以需要手动刷新
    //检查缓冲区大小.
    
}