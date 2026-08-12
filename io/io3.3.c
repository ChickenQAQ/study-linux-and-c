//编写一个程序，令其功能尽量向命令 “cp” 靠近。
#include<pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/mman.h>


int copy_file(int src_fd,int dst_fd)
{
    char buf[1024];
    while(1)
    {
        ssize_t n = read(src_fd,buf,sizeof(buf));
        if(n == -1)
        {
            perror("read");
            exit(1);
        }
        if(n == 0)
        {
            break;
        }
        ssize_t written =0;
        while(written < n)
            {
                ssize_t w = write(dst_fd,buf+written,n-written);
                if(w == -1)
                {
                    perror("write");
                    exit(1);
                }
                written += w;
            }
    }
    close(src_fd);
    close(dst_fd);
    return 0;
}
int copy_dir(const char *src_dir,const char *dst_dir)
{
    mkdir(dst_dir,0777);
    DIR *dir = opendir(src_dir);
    if(dir == NULL)
    {
        perror("opendir");
        exit(1);
    }
    struct dirent *d;
    while((d=readdir(dir))!=NULL)
    {
        if(strcmp(d->d_name,".")==0||strcmp(d->d_name,"..")==0)
        {
            continue;
        }
        char src_path[1024]={0};
        char dst_path[1024]={0};
        sprintf(src_path,"%s/%s",src_dir,d->d_name);
        sprintf(dst_path,"%s/%s",dst_dir,d->d_name);
        struct stat st;
        if(stat(src_path,&st)==-1)
        {
            perror("stat");
            exit(1);
        }
        if(S_ISDIR(st.st_mode))
        {
            copy_dir(src_path,dst_path);
        }
        else
        {
            int src_fd = open(src_path,O_RDONLY);
            int dst_fd = open(dst_path,O_WRONLY|O_CREAT|O_TRUNC,0644);
            copy_file(src_fd,dst_fd);
        }
    }
    closedir(dir);
    return 0;
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        exit(1);
    }
    struct stat st;
    if (stat(argv[1], &st) == -1)
    {
        perror("open");
        exit(1);
    }
    if(S_ISDIR(st.st_mode))
    {
        copy_dir(argv[1],argv[2]);
    }
    else
    {
        int src_fd = open(argv[1],O_RDONLY);
        int dst_fd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
        copy_file(src_fd,dst_fd);
    }
    
}