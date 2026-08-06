#include <stdio.h>
float myPower(float x, int N)
{
    if (N > 0)
    {
        return x*myPower(x,N-1);
       
    }
    else if(N<0)
    {
        return 1.0f/myPower(x,-N);
        
    }
    else return 1.0f;
}
int main(int argc, char const *argv[])
{
    float a = myPower(5,2);
    printf("%f",a);
    return 0;
}
