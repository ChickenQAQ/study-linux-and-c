#include <stdio.h>
#include "a.h"

void func()
{
    printf("我是%s中的函数%s\n", __FILE__, __FUNCTION__);
}