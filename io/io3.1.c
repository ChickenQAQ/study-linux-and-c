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
   if(argc != 2)
   {
      printf("Usage: %s <filename>\n", argv[0]);
      exit(1);
   }
   FILE * fd = fopen(argv[1],"r");
   if(fd == NULL)
   {
       perror("open");
       exit(1);
   }
   int size = 0;
   char buffer;
   while(1)
   {
       int n = fread(&buffer, 1, 1, fd);
       if(n==0||feof(fd))
       {
        printf("read %d bytes\n", size);
        fclose(fd);
        break;
       }
       size++;
   }
   return 0;
}