#ifndef HOME_H
#define HOME_H
#include <stdbool.h>
typedef struct 
{
    int capacity;//容量
    int last;//下标
    int * data; //顺序表
}seq;

//删除顺序表元素
extern bool delete_data (seq *list,int n);
//遍历展示顺序表元素
extern void showlist(seq * list);
//插入元素到顺序表头
extern  bool insert(seq* list , int data);
//新建顺序表
extern seq * initlist(int n);//n为顺序表大小
#endif