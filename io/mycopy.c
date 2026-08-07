#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("Usage: %s src dst\n",argv[0]);
        exit(1);
    }
    FILE * fp_src = fopen(argv[1],"r");
    if(fp_src==NULL)
    {
        fprintf(stderr,"fopen(%s) failed: %s\n",argv[1],strerror(errno));
        exit(1);
    }
    FILE * fp_dst = fopen(argv[2],"w");
    if(fp_dst==NULL)
    {
        fprintf(stderr,"fopen(%s) failed: %s\n",argv[2],strerror(errno));
        exit(1);
    }
    int c;
    while(1)
    {
        c=fgetc(fp_src);
        if(c==EOF)
        {
            if(feof(fp_src))//到达文件的尾巴,应该break掉
                break;
            if(ferror(fp_src))//遇到错误,应该输出错误信息
            {
                perror("fgetc() failed");
                break;
            }
        }
        fputc(c,fp_dst);
    }
    fclose(fp_src);
    fclose(fp_dst);
    return 0;

}
