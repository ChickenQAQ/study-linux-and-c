#include<stdio.h>
#include<stdlib.h>

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("Usage: %s openfile\n",argv[0]);
        exit(1);
    }
    FILE * fp = fopen(argv[1],"r");
    if(fp==NULL)
    {
        perror("fopen() failed");
        exit(1);
    }
    while(1)
    {
        int c = fgetc(fp);
        if(c==EOF)
            break;
       fprintf(stdout,"%c",c);
    }
    fclose(fp);
    return 0;
}