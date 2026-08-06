#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 快速排序
void swap(int *a,int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
int  fast_sort(int *data,int left,int right)
{
    // 1. 基础情况：如果数组为空或只有一个元素，直接返回
    if (right <= left) return 0;
    
    int pivot = data[left];      // 选第一个元素作为基准
    int i = left, j = right;       // 注意 j 初始为 right（不是 right-1）
    
    while (i < j) {
        // 从右向左找第一个 <= pivot 的元素（跳过 > pivot 的）
        do {
            j--;
        } while (i < j && data[j] > pivot);
        
        // 从左向右找第一个 >= pivot 的元素（跳过 < pivot 的）
        do {
            i++;
        } while (i < j && data[i] < pivot);
        
        // 如果 i 和 j 还没相遇，交换这两个"错位"的元素
        if (i < j) {
            swap(&data[i], &data[j]);
        }
    }
    
    // 最后将基准放到正确位置（j 指向的位置）
    // 2. 递归处理左右子数组
    fast_sort(data, left, j - 1);  // 递归排序左半部分（注意是 j-1）
    fast_sort(data, j + 1, right); // 递归排序右半部分（注意是 j+1）
    
    // 3. 基准放到正确位置
    swap(&data[left], &data[j]);  // ✅ 交换基准    到正确位置
    return j;
}

void sort(int *a,int left,int right)
{
    if(left >= right)
        return;
    int pivot = fast_sort(a,left,right);
    sort(a,left,pivot-1);
    sort(a,pivot+1,right);
}
int main()
{
    int a[] = {1,3,2,4,5,6,7,8,9};
    int size = sizeof(a)/sizeof(a[0]);
    sort(a,0,size-1);
    for(int i=0;i<size;i++)
    {
        printf("%d ",a[i]);
    }
    printf("\n");
    return 0;
}