#include <stdio.h>
#include<pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }
    FILE *input_file = fopen(argv[1], "r");
    if(input_file == NULL)
    {
        perror("Error opening input file");
        exit(1);
    }
    FILE *output_file = fopen(argv[2], "w");
    if(output_file == NULL)
    {
        fprintf(stderr, "Error opening output file");
        exit(1);
    }
    char buffer[1024];
    while(1)
    {
        long a = ftell(input_file);
        int n = fread(buffer, 20,5, input_file);
        if(n==5)
        {
            fwrite(buffer, 20, 5, output_file);
        }
        else
        {
            if(feof(input_file))
            {
                long b = ftell(input_file);
                fwrite(buffer, b-a,1, output_file);
                break;
            }
            if(ferror(input_file))
            {
                perror("Error reading input file");
                exit(1);
            }
        }
    }
    return 0;
}