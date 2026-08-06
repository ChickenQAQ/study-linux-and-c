#include<stdio.h>
#include "myhead.h"
double mypow(int a , int n )
{
  if(n<0)
  {
    return  1/mypow(a,-(n-1))*a ;
  }
  if(n==0)
  {
    return 1;
  }
  if(n>0)
  {
    return mypow(a,n-1)*a;
  }
}
void recur(char s[], int len)
{
  
}

int main(int argc, char const *argv[])
{
  printf("%lf",mypow(2,-1));

}
