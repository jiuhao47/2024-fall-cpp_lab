void main()
{
	int obc a[8] = {0,1,2,3,4,5,6,7};
	int obc b[8] = {8,9,10,11,12,13,14,15};

	input();
	output_var = a[input_var];
	output();
	input();
	output_var = b[input_var];
	output();
}

/*
input:-1
Runtime Error: array 'a' out of bound check error at Line:7 Pos:14
*/

/*
input:0
output:0
input:8
Runtime Error: array 'b' out of bound check error at Line:10 Pos:14
*/

/*
input:7
output:7
input:-1
Runtime Error: array 'b' out of bound check error at Line:10 Pos:14
*/

/*
input:0
output:0
input:0
output:8
*/