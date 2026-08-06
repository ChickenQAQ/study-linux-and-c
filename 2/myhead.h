bool f (int a)
{
    if(a==2)
    return true;
    for(int i=2;i<a;i++)
    {
        if(a%i==0)
        return false;
    }
    return true;
}