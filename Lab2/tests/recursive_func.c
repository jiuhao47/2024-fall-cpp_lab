int obc result[10];

void func()
{
    result[arg0] = arg0;
    arg0 = arg0 - 1;
    if (arg0 >= 0) {
        func();
    }
}

void main()
{
    input();
    arg0 = input_var - 1;
    int cnt = arg0;
    func();
    while(cnt >= 0)
    {
        output_var = result[cnt];
        output();
        cnt = cnt - 1;
    }
}

/*
input:5
output:4
output:3
output:2
output:1
output:0
*/

/*
input:11
Runtime Error: array 'result' out of bound check error at Line:5 Pos:4
*/

/*
input:0
Runtime Error: array 'result' out of bound check error at Line:5 Pos:4
*/