void func()
{
    int a = arg0;

    if(a % 2 == 0)
    {
        a = a / 2;
    }
    else {
        a = 3 * a + 1;
    }

    arg0 = a;
}

void main()
{
    int obc x[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    arg0 = 3;
    func();
    output_var = x[arg0];   // arg0 = [1, 10]
    output();
}
