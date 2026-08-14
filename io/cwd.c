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
    if (argc != 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        exit(1);
    }
    char *cwd1 = getcwd(NULL, 0);
    printf("Current working directory: %s\n", cwd1);
    // Open the directory
    DIR * dir = opendir(argv[1]);
    if (dir == NULL) {
        printf("Error opening directory: %s\n", strerror(errno));
        exit(1);
    }
    chdir(argv[1]);
    char *cwd = getcwd(NULL, 0);
    printf("Current working directory: %s\n", cwd);
    free(cwd);
    free(cwd1);
    return 0;
}