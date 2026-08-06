#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define DATATYPE struct student
typedef struct student
{
    int age;
    char * name;
}student;
#include "list.h"
bool equaldata( struct student * temp, student *n)
{
    if (temp == NULL && n == NULL) return true;
    if (temp == NULL || n == NULL) return false;
     return (temp->age == n->age) && (strcmp(temp->name, n->name) == 0);

}

void showdata(student *list)
{
    printf("名字为%s,年龄为%d\n",list->name,list->age);
}
int main(int argc, char const *argv[])
{
   node * list = initlist();
   if(list!=NULL)
   printf("初始化成功\n");
   student jack = {18,"jack"};
   student chicken = {18,"chicken"};
   node * new = newnode(jack);
   insertlist(list,new);
   node * new1 = newnode(chicken);
   insertlist(list,new1);
   showlist(list,showdata);
   if(deletelist(list,jack,equaldata))
   {
    printf("删除链表成功\n");
   }
    showlist(list,showdata);
    findlist(list,chicken,equaldata,showdata);
    destroylist(list);
    return 0;
}
