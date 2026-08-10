//无重复字符的最长字串,暴力法
int lengthOfLongestSubstring(char * s){
    int len = strlen(s);
    int max = 0;
    int i,j,tmp;
    int str[128]={0};
    for(i=0;i<len;i++)
    {
        memset(str,0,sizeof(str));
        tmp=0;
        for(j=i;j<len;j++)
        {
            if(str[s[j]]==0)
            {
                str[s[j]]++;
                tmp++;
            }
            else
            {
                break;
            }
        }
        max=max>tmp?max:tmp;
    }
    return max;
}
//无重复字符的最长字串,移动窗口法
int lengthOfLongestSubstring(char * s){
    int left = 0;
    int right = 0;
    int len = strlen(s);
    int max = 0;
    int str[128]={0};
    for(right=0;right<len;right++)
    {
       str[s[right]]++;
            while(str[s[right]]>1)
            {
                str[s[left]]--;
                left++;
            }
        max=max>(right-left+1)?max:(right-left+1);
    }
    return max;
}