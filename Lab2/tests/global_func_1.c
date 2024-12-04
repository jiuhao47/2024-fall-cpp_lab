void g()
{
    int yy_yy_yy[3] = {0, 1, -1};
    output_var = yy_yy_yy[1];
    output();
}

void f()
{
    int x;
    x = 3;
    output_var = x;
    output();
    g();
}

void main()
{
    f();
    g();
}

/*
output:3
output:1
output:1
*/
