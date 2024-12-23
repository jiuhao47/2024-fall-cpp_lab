int _time;

void times()
{
    _time = 1 + _time;
    output_var = _time;
    output();
}

void func()
{
    input();
    int x = input_var;
    
    if (x < 0)
    {
        output_var = -1;
        output();
    }
    else {
        while (x >= 0)
        {
            times();
            x = (x - 1);
        }
    }

}

void main()
{
    func();
}

/*
input:-23333
output:-1
*/

/*
input:3
output:1
output:2
output:3
output:4
*/