void main()
{
    int idx = -1;
	int a = 4;
	int aa = 5;
	int b = -1;
	int bb = 8;
	int cond = -1;
	int obc x[8] = {0, 1, 2, 3, 4, 5, 6, 7};

	input();
	cond = input_var;

	if(cond % 3 == 1)
    {
	    a = aa;
    }
	else {
	    b = bb;
	}

	idx = a;
	output_var = x[idx];    // idx = [4, 5]
	output();

    idx = b;
    output_var = x[idx];    // idx = [-1, 8]
    output();
}
