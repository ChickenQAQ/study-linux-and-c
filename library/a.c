////////////////////////////////////////////////////////
//
//  Copyright(C), 2005-2022, GEC Tech. Co., Ltd.
//  www.yueqian.com.cn  ALL RIGHT RESERVED
//
//  文件: a.c
//  描述: 具有某个功能的代码模块，用以演示库的相互依赖
//
///////////////////////////////////////////////////////

#include <stdio.h>

void fa()
{
    printf("我是%s\n", __FUNCTION__);
}
