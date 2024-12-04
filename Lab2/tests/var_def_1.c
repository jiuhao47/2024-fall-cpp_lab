int x = 1;

void main()
{
    output_var = x;
    output();
    int x = 2;
    output_var = x;
    output();
    {
        int x = 3;
        output_var = x;
        output();
        {
            int x = 4;
            output_var = x;
            output();
            {
                int x[20] = { 2, 3, 4, 5 };
                output_var = x[3];
                output();
            }
            output_var = x;
            output();
        }
        output_var = x;
        output();
    }
    output_var = x;
    output();
    {
        int x = -0x13;
        output_var = x;
        output();
    }
}

/*
output:1
output:2
output:3
output:4
output:5
output:4
output:3
output:2
output:-19
*/