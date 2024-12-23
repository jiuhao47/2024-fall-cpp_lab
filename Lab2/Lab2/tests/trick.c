void main()
{
	int x[0x8];
	int index;
	int addr;
	
	input();
	index = input_var;
	while (index >= 0)
	{
		input();
		addr = input_var;
		x[index] = addr;
		input();
		index = input_var;
	}
}

/*
input:12
input:67108864
input:13
input:0
input:-1
*/