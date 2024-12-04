const int x[4] = {1, 2, 3};

void main()
{
	x[0] = x[1];
}

/*
5:1: assignment of read-only variable 'x'
Semantic failed. Exiting.
*/