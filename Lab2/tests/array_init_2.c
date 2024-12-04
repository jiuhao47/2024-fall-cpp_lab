void main()
{
    int obc x[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    input();
    output_var = x[input_var];
    output();
}

/*
input:0
output:0
*/

/*
input:8
output:8 
*/

/*
input:9
output:0 
*/

/*
input:-1
Runtime Error: array 'x' out of bound check error at Line:5 Pos:17
*/

/*
input:10
Runtime Error: array 'x' out of bound check error at Line:5 Pos:17
*/