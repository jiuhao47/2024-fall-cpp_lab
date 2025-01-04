void func()
{
    int cond = 0x13;
    int a = 0;
    int obc x[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    input();
    cond = input_var;

    if(cond % 2 == 1)
    {
        a = 1337;
    }
    else {
        a = -1;
    }

    if(cond % 2 != 1)
    {
        if(a == -1)
            a = 10;
    }
    else {
        if(a == 1337)
            a = 5;
    }

    output_var = x[a];  // a = [-1, 5, 10, 1337]
    output();
}

void main()
{
    func();
}
