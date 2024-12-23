void funcA()
{
    arg0 = 3;
}

void funcB()
{
    input();
    arg0 = input_var;
}

void main()
{
    int a = -1;
    int b = -1;
    int obc x[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    input();
    a = input_var;
	output_var = x[a];  // a = TOP
	output();

	input();
	a = input_var;
	arg0 = a;
	funcA();
	a = arg0;
    output_var = x[a];  // a = [3]
    output();

    input();
    a = input_var;
    arg0 = a;
    funcB();
    a = arg0;
    output_var = x[a];  // a = TOP
    output();
}
