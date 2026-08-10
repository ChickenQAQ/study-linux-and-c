#include<stdio.h>
struct people
{
    char name[20];
    int grade;
};

int main ()
{
    FILE *fp = fopen("record.txt","r");
    if(fp == NULL)
    {
        printf("Failed to open file\n");
        return 1;
    }
    struct people p[100];
    int num=0;
    while(fscanf(fp,"%s %d",p[num].name,&p[num].grade) == 2)
    {
        num++;
    }
    fclose(fp);
    for(int i = 0;i<num;i++)
    {
        for(int j = i+1;j<num;j++)
        {
            if(p[i].grade > p[j].grade)
            {
                struct people temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }
    for(int i=0;i<num;i++)
    {
        printf("%d\t",p[i].grade);
    }
    printf("\n");
    return 0;
}