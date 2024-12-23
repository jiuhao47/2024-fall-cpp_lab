int ga = 0x13;
int gb = 7;

void main()
{
    int idx = -1;
	int a = 3;
	int b = 10;
	int obc x[8] = {0, 1, 2, 3, 4, 5, 6, 7};

	idx = b + a;
	output_var = x[idx];    // idx = [13]
	output();

    idx = b - a;
    output_var = x[idx];    // idx = [7]
    output();

    idx = b * a;
    output_var = x[idx];    // idx = [30]
    output();

    idx = b / a;
    output_var = x[idx];    // idx = [3]
    output();

    idx = b % a;
    output_var = x[idx];    // idx = [1]
    output();

    {
        idx = gb + ga;
        output_var = x[idx];    // idx = [26]
        output();

        idx = gb - ga;
        output_var = x[idx];    // idx = [-12]
        output();

        idx = gb * ga;
        output_var = x[idx];    // idx = [133]
        output();

        idx = gb / ga;
        output_var = x[idx];    // idx = [0]
        output();

        idx = gb % ga;
        output_var = x[idx];    // idx = [7]
        output();
    }
}
