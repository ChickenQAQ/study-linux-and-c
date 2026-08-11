//2996.大于等于顺序前缀和的最小缺失整数
#include <stdio.h>
int minMissingValue(int* nums, int numsSize){
    int max = nums[0];
    int len =0;
    int i = 0;
    for(; i < numsSize-1; i++)
    {
        int j = i+1;
        if(nums[j] - nums[i] == 1)
        {
            len++;
            max+=nums[j];
        }
        else 
        break;
    }
    while(i!= numsSize)
    {
        if(max==nums[i])
        {
            max++;
            i=0;
        }
        else
        i++;
    }
    return max;
}
int main()
{
    int nums[] = {1,2,3,2,5};
    int numsSize = sizeof(nums)/sizeof(nums[0]);
    printf("%d\n",minMissingValue(nums,numsSize));
    return 0;
}