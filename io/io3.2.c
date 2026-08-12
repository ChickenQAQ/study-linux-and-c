//编写一个程序，令其功能向命令 “ls -l” 靠近。

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


int main(int argc,char *argv[])
{
    if(argc !=2)
    {
        printf("用法：%s 目录路径\n", argv[0]);
        return -1;
    }
    DIR *dir = opendir(argv[1]);
    if(dir == NULL)
    {
        perror("opendir file");
        return -1;
    }
    struct dirent *d;
    struct stat st;
    char fullpath[512];
    while((d = readdir(dir)) != NULL)
    {
        if(d->d_name[0] == '.')
            continue;
        sprintf(fullpath, "%s/%s", argv[1], d->d_name);
        if(lstat(fullpath, &st) == -1)
        {
            perror("stat file");
            return -1;
        }
        char * color = "";
        if(S_ISDIR(st.st_mode))
            color = "\033[1;34m";
        else if(S_ISLNK(st.st_mode))
            color = "\033[1;36m";
        else if(S_ISREG(st.st_mode)&&(st.st_mode & 0111))
            color = "\033[1;32m";
        char *t =ctime(&st.st_mtime);
        t[strlen(t)-1] = '\0';
         printf("文件名\t大小\t用户ID\t组ID\t修改时间\n");
        printf("%s%s\033[0m\t", color, d->d_name);
        printf("%ld\t%d\t%d\t%s\n", st.st_size, st.st_uid, st.st_gid, ctime(&st.st_mtime));
    }
    closedir(dir);
    return 0;
}