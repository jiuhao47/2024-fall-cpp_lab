int z[10] = {-0, -1, -2, -3, -4, -5, -2*-3-12, -7, -8, -9};

void main()
{
    int x = -0x18;
    int y = x*x/(x+0x8);

    y = (x+y)/(x-y);
    x = x%y;
    y = x + y*(x+y) + (x%-y)+(x);
    
    {
        // y = 33
        int x[10]={0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        output_var = x[y/11];
        output();
        {
            output_var = x[y/-11+8];
            output();
            // x[0] 
            int a = 3;
            x[6/2%3] = x[1]*x[2] + x[7]%3 + 0x80%x[-1+10] + x[(a*3+1)/2]*(a*a+1)/2;
            {
                output_var = z[y/11*2];
                output();
                output_var = x[0];
                output();
            }
        }
    }
}

/*
output:3
output:5
output:-6
output:30
*/