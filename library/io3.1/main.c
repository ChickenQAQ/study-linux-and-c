#include "fileio.h"

int main(int argc , char *argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    int fp = my_open(argv[1], O_RDWR,0);
    char buffer[100];
    int len = my_read(fp, buffer, 100);
    printf("Read %d bytes: %s\n", len, buffer);
    int n = my_write(fp, "Hello,World!", 13);
    my_lseek(fp, 0, SEEK_SET);
    char buffer2[100];
    int len2 = my_read(fp, buffer2, 100);
    printf("Read %d bytes: %s\n", len2, buffer2);
    int seek = my_lseek(fp, 0, SEEK_END);
    printf("Seek to %d\n", seek);
    struct stat statbuf;
    int mystat = my_stat(argv[1], &statbuf);
    printf("File size: %ld\n", statbuf.st_size);
    my_close(fp);
    return 0;
}