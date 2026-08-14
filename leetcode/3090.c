//每个字符最多出现两次的最长子字符串
//滑动窗口法
int maximumlengthSubstring(char *s)
{
    int len = strlen(s);
    int a[26]={0};
    int left =0;
    int max =0;
    int count =0;
    for(int right =0;right<len;right++)
    {
        a[s[right]-'a']++;
        count++
        while(a[s[right]-'a']>2)
        {
            a[s[left]-'a']--;
            left++;
            count--;
        }
        if(count>max)
        {
            max = count;
        }
    }
    return max
}