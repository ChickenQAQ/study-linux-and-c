//由单个字符重复的最长字符串

//1.暴力法，我的方法，超时不可取
/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
//  int find_max (char * s)
//  {
//     int len =strlen(s);
//     if(len == 0) return 0;
//     if(len == 1) return 1;
//     int max=0;
//     char n = s[0];
//     int count = 1;
//     for(int i = 1;i<len;i++ )
//     {
//         if(s[i]==n)
//             count++;
//         else
//         {
//             if(count>max)
//                 max =count;
//             count =1;
//             n=s[i];
//         }
//     }
//     if(count > max)
//         max = count;
//     return max;
//  }
// int* longestRepeating(char* s, char* queryCharacters, int* queryIndices, int queryIndicesSize, int* returnSize) {
//     int * result = malloc(queryIndicesSize*sizeof(int));
//     for(int i =0;i<queryIndicesSize;i++)
//     {
//         int a = queryIndices[i];
//         s[a]=queryCharacters[i];
//         result[i] = find_max(s);
//     }
//     *returnSize =queryIndicesSize;
//     return result;

// }

//2.类二叉树法，又称线段树法
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int pre; //前缀
    int suf; //后缀
    int max_len; //区间最大长度
    char left_ch;//区间最左字符
    char right_ch;//区间最右字符
}Node;

Node * tree;
char* s_global;//把传入进来的字符串s，赋值给全局指针

int max3(int a,int b,int c)
{
    return a>b?(a>c?a:c):(b>c?b:c);
}
//合并左右子树
void push_up(int node,int left,int right)
{
    int mid =(left+right)/2;
    int lson = node *2;
    int rson = node *2+1;
   //继承两端字符，访问子节点 lson rson
    tree[node].left_ch = tree[lson].left_ch;
    tree[node].right_ch = tree[rson].right_ch;

    //计算pre前缀
    if(tree[lson].pre == mid-left+1 && tree[lson].right_ch == tree[rson].left_ch)
    {
        tree[node].pre = tree[lson].pre + tree[rson].pre;
    }
    else
    {
        tree[node].pre = tree[lson].pre;
    }

    //计算suf后缀
    if(tree[rson].suf == right - (mid+1) +1 && tree[rson].right_ch == tree[lson].right_ch)
    {
        tree[node].suf = tree[rson].suf + tree[lson].suf;
    }
    else
    {
        tree[node].suf = tree[rson].suf;
    }

    int cross = 0;
    if(tree[lson].right_ch == tree[rson].left_ch)
    {
        cross = tree[lson].suf + tree[rson].pre;
    }
    tree[node].max_len = max3(tree[lson].max_len, tree[rson].max_len, cross);
}
//建树
void build_tree(int node,int left,int right)
{
    if(left == right)
    {
        tree[node].pre = 1;
        tree[node].suf = 1;
        tree[node].max_len = 1;
        tree[node].left_ch = s_global[left];
        tree[node].right_ch = s_global[left];
        return;
    }
    int mid = (left+right)/2;
    build_tree(node*2,left,mid);
    build_tree(node*2+1,mid+1,right);
    push_up(node,left,right);
}
//单点修改，pos位置改成ch
void update(int node,int left,int right,int pos,char ch)
{
    if(left == right)
    {
        tree[node].pre = 1;
        tree[node].suf = 1;
        tree[node].max_len = 1;
        tree[node].left_ch = ch;
        tree[node].right_ch = ch;
        return;
    }
    int mid = (left+right)/2;
    if(pos<=mid)
    {
        update(node*2,left,mid,pos,ch);
    }
    else
    {
        update(node*2+1,mid+1,right,pos,ch);
    }
    push_up(node,left,right);
}

int* longestRepeating(char* s, char* queryCharacters, int* queryIndices, int queryIndicesSize, int* returnSize) {
    int len = strlen(s);
    s_global = s;
    tree = malloc(4*len*sizeof(Node));
    build_tree(1,0,len-1);
    int * result = malloc(queryIndicesSize*sizeof(int));
    for(int i =0;i<queryIndicesSize;i++)
    {
        int pos = queryIndices[i];
        char ch = queryCharacters[i];
        update(1,0,len-1,pos,ch);
        result[i] = tree[1].max_len;
    }
    free(tree);
    *returnSize =queryIndicesSize;
    return result;

}