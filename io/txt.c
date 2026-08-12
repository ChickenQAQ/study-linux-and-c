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

int main(int argc, char *argv[])
{
    if(argc !=2)
    {
        printf("用法：%s 目录路径\n", argv[0]);
        return -1;
    }
    DIR *dp = opendir(argv[1]);
    if(dp == NULL)
    {
        perror("opendir file");
        return -1;
    }
    struct dirent *dirp;
    // while((dirp = readdir(dp)) != NULL)
    // {
    //     if(dirp->d_name[0] == '.')
    //         continue;
    //     printf("%s\n",dirp->d_name);
    // }
    char fullpath[512];
    struct stat st;
    while((dirp = readdir(dp)) != NULL)
    {
        if(dirp->d_name[0] == '.')
            continue;
        snprintf(fullpath,sizeof(fullpath),"%s/%s",argv[1],dirp->d_name);
        if(stat(fullpath,&st) == -1)
        {
            perror("stat file");
            continue;
        }
        printf("%8ld %s\n",st.st_size,dirp->d_name);
    }
    closedir(dp);
    return 0;
}