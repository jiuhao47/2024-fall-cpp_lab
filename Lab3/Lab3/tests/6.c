void func()
{
    int idx = -1;
    int a = 4;
    int cond = -1;
    int obc x[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    input();
    cond = input_var;
    if(cond > 3)
    {
        a = 2;
    }
    else {
        a = 6;
    }
    idx = a;
    output_var = x[idx];    // idx = [2, 6]
    output();

    input();
    cond = input_var;
    if(cond < 3)
    {
        a = 2;
    }
    else {
        a = 8;
    }
    idx = a;
    output_var = x[idx];    // idx = [2, 8]
    output();

    input();
    cond = input_var;
    if(cond == 3)
    {
        a = -1;
    }
    else {
        a = 6;
    }
    idx = a;
    output_var = x[idx];    // idx = [-1, 6]
    output();

    input();
    cond = input_var;
    if(cond % 3 == 0)
    {
        a = -1;
    }
    else {
        a = 8;
    }
    idx = a;
    output_var = x[idx];    // idx = [-1, 8]
    output();
}

void main() {
}
