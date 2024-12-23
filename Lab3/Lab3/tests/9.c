// complex

int obc x[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

void funcA()
{
    int a = arg0;

    a = arg0*2+1;

    output_var = x[a]; // a = [1, 5, 9, 13, 15]
    output();
}

void funcB()
{
    int b = arg0;

    b = arg0*2-1;

    output_var = x[b];  // b = [-1, 3, 7, 11, 13]
    output();
}

void main()
{
    int x = 0;
    int cond = -1;

    input();
    cond = input_var;
    if(cond == 0x66)
    {
        input();
        cond = input_var;
        if(cond == 0x6c)
        {
            input();
            cond = input_var;
            if(cond == 0x61)
            {
                input();
                cond = input_var;
                if(cond == 0x67)
                {
                    x = 0;
                }
                else {
                    x = 2;
                }
            }
            else {
                x = 4;
            }
        }
        else {
            x = 6;
        }
    }
    else {
        x = 7;
    }

    arg0 = x;
    funcA();

    arg0 = x;
    funcB();

}
