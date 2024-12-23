void funcA()
{
    int x = -1;
    int cond = -1;

    input();
    cond = input_var;

    if(cond % 7 == 5)
    {
        arg1 = 9;
    }
    else {
        arg1 = 10;
    }
}

void funcB()
{
    int x = -1;
    int cond = -1;

    input();
    cond = input_var;

    if(cond % 7 == 5)
    {
        arg1 = 9;
    }
    else {
        if(cond < 0)
        {
            cond = -cond;
            while(cond >= 0)
            {
                x = 1 + x;
                cond = cond - 1;
            }
            arg1 = x;
        }
    }
}

void main()
{
    int a = -1;
    int obc x[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	input();
	a = input_var;
	arg0 = a;
	funcA();
	a = arg1;
    output_var = x[a];  // a = [9, 10]
    output();

    input();
    a = input_var;
    arg0 = a;
    funcB();
    a = arg1;
    output_var = x[a];  // a = TOP
    output();
}
