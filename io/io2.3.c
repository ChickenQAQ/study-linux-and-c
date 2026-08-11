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
#include <time.h>
#include <signal.h>

static  long line = 0;//行号
static int g_exit = 0;
void sigint_handler(int sig)//识别ctrl c
{
    (void)sig;
    g_exit = 1;
}
long getLine (FILE *fp)
{
    char buf[256];
    if(fp!=NULL)
    {
        while(fgets(buf,sizeof(buf),fp)!=NULL)
        {
            line++;
        }
        return line;
    }
    else
    return 0;
}
int main(int avgc,char *argv[])
{
    signal(SIGINT, sigint_handler);//注册信号
    FILE *fp_r=fopen("time.txt","r");
    long line = getLine(fp_r);
    if(fp_r != NULL)
    {
        fclose(fp_r);
    }
    FILE *fp=fopen("time.txt","a");
    getLine(fp);
    while(!g_exit)
    {
        sleep(1);
        time_t tim  = time(NULL);
        fprintf(fp,"%ld",line);
        fprintf(fp,"%s",ctime(&tim));
        line++;
        fflush(fp);
    }
    fclose(fp);
    return 0;
}