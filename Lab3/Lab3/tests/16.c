void funcA0()
{
    int a = arg1;

    if(a % 2 == 0)
    {
        a = a / 2;
    }
    else {
        a = a / 3;
    }

    arg1 = a;
}

void funcA1()
{
    int a = arg1;

    if(a % 2 == 0)
    {
        a = a / 2;
    }
    else {
        a = a / 3;
    }

    arg1 = a;
}

void funcA2()
{
    int a = arg1;

    if(a % 2 == 0)
    {
        a = a / 2;
    }
    else {
        a = a / 3;
    }

    arg1 = a;
}

void funcA3()
{
    int a = arg1;

    if(a % 2 == 0)
    {
        a = a / 2;
    }
    else {
        a = a / 3;
    }

    arg1 = a;
}

void funcB()
{
    int a = arg1;

    if(a % 2 == 1)
    {
        a = a * 2;
    }
    else {
        a = a * 3;
    }

    arg1 = a;
}

void go()
{
    int obc x[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int a = arg2;
    int b = 1;
    int time = 1;

    arg1 = a;
    funcA0();
    a = arg1;

    arg1 = a;
    funcA1();
    a = arg1;

    arg1 = a;
    funcA2();
    a = arg1;

    arg1 = a;
    funcA3();
    a = arg1;

    output_var = x[a];  // KSet size 3: a = TOP | KSet size 8: a = [1, 2, 3, 5]
    output();

    while(time > 0)
    {
        arg1 = b;
        funcB();
        b = arg1;
        time = time - 1;
    }

    output_var = x[b];  // KSet size 3: b = TOP | KSet size 8: TOP
    output();
}

void main()
{
    arg2 = 82;
    go();
}
