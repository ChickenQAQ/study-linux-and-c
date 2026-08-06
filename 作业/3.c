#include <stdio.h>
void wendu(unsigned int n)
{
    n = n << 24;
    printf("温度为%u\n", n >> 24);
}
void shidu(unsigned int n)
{
    n = n << 16;
    printf("湿度为%u\n", n >> 24);
}
void men(unsigned int n)
{
    n = n << 12;
    printf("%#x\n", n >> 28);
}
void deng(unsigned int n)
{
    n = n << 8;
    printf("%#x\n", n >> 28);
}

int main()
{
    unsigned int a, b, c;
    a = 0x12344520;
    b = 0xff004B1C;
    c = 0x00553C1E;
    wendu(a);
    shidu(a);
    men(a);  // 0100
    deng(a); // 0011
    wendu(b);
    shidu(b);
    men(b);  // 0000
    deng(b); // 0000
    wendu(c);
    shidu(c);
    men(c);  // 0101
    deng(c); // 0101
}
