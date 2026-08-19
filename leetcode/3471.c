//3471找出最大的几近缺失整数
int largestInteger(int* nums, int numsSize, int k) {
    int max =-1;
    if(k==1)
    {
        int n[51]={0};
        for(int i = 0;i<numsSize;i++)
                n[nums[i]]++;
        for(int i =0;i<51;i++)
        {
            int count = -1;
            if(n[i]==1)
            {
                count = i;
                if(count>max)
                    max = count;
            }
        }
    }
    else if(k==numsSize)
    {
        for(int i = 0;i<numsSize;i++)
        {
            if(nums[i]>max)
                max = nums[i];
        }
    }
    else
    {
        if(nums[0]==nums[numsSize-1])
            return -1;
        for(int i =1;i<numsSize-1;i++)
        {
            if(nums[i]==nums[0])
                nums[0]=-1;
            if(nums[i]==nums[numsSize-1])
                nums[numsSize-1]=-1;
        } 
        if(nums[0]>nums[numsSize-1])
            max = nums[0];
        else
        max = nums[numsSize-1];  
    }
    return max;
}