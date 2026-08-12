//leetcode2958:最多k个重复元素的最长子数组
//思路：滑动窗口
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef struct hashmap
{
    int key;
    int cnt;
}Hash;
int max_length(int *nums, int numsSize, int k) 
{
    int left = 0 ,right = 0;
    int hashsize = numsSize;
    Hash *hash = calloc(hashsize,sizeof(Hash));
    for(int i = 0;i<numsSize;i++)
    {
        hash[i].key = -1;
        hash[i].cnt = 0;
    }
    int max =0;
    for(int right = 0;right<numsSize;right++)
    {
        int val = nums[right];
        int idx = val%hashsize;
        if(hash[idx].key!=-1&&hash[idx].key!=val)
        {
            idx =(idx+1)%hashsize;
        }
        else
        {
            hash[val].cnt = 1;
            hash[val].key = val;
        }
        while(1)
        {
            int find_idx = val%hashsize;
            while(hash[find_idx].key!=val)
            {
                find_idx = (find_idx+1)%hashsize;
            }
            if(hash[find_idx].cnt>k)
            {
                int left_val = nums[left];
                int lidx = left_val%hashsize;
                while(hash[lidx].key!=left_val)
                {
                    lidx = (lidx+1)%hashsize;
                }
                hash[lidx].cnt--;
                left++;
            }
            else
            {
                break;
            }
        }
        max = fmax(max,right-left+1);
    }
    return max;
}
int main()
{
    int nums[] = {1,2,4,4,3,3,1,4,4,4,4};
    int k = 3;
    int numsSize = sizeof(nums)/sizeof(nums[0]);
    int res = max_length(nums,numsSize,k);
    printf("%d\n",res);
    return 0;
}