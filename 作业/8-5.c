#include <stdio.h>
#include <math.h>

#define SIZE 3

void transform(double source[], double target[],
               int size, double (*func)(double))
{
	int i;
	for(i=0; i<size; i++)
		target[i] = func(source[i]);
}

double func1(double i)
{
	return i+1;
}

double func2(double i)
{
	return i*2;
}

void show(double ar[])
{
	int i;
	for(i=0; i<SIZE; i++)
	{
		printf("%f\t", ar[i]);
	}
	printf("\n");
}

int main(void)
{
    // 源数组
	double source[SIZE] = {0.1, 0.2, 0.3};
    // 目标数组
	double target[SIZE];

    // 使用自定义函数 func1，将源数组转入目标数组
	transform(source, target, SIZE, func1);
	show(target);
	printf("--------------\n");

    // 使用自定义函数 func2，将源数组转入目标数组
	transform(source, target, SIZE, func2);
	show(target);
	printf("--------------\n");

    // 使用库函数sin，将源数组转入目标数组
	transform(source, target, SIZE, sin);
	show(target);
	printf("--------------\n");

    // 使用库函数cos，将源数组转入目标数组
	transform(source, target, SIZE, cos);
	show(target);

	return 0;
}