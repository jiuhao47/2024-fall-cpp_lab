void hunc()
{
    int x, y;
    
    input();
    x = input_var;
    input();
    y = input_var;
    if (x > 0x10)
    {
        if (0x201+0x80/8 >= y)
        {
            output_var = 666;
            output();
        }
        else {
            output_var = 233333;
            output();
        }
    }
}

void gunc()
{
    hunc();
}

void func()
{
    gunc();
}

void main()
{
    func();
}

/*
input:17
input:529
output:666
*/

/*
input:16
input:529
*/

/*
input:17
input:530
output:233333
*/