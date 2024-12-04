int x = 1;

void main()
{
    int x = 2;
    {
        int x = 3;
    }
    {
        int x = -0x13;
        {
            {
                {
                    {
                        {
                            {
                                int x = 0x13;
                                int x[] = {1, 2, 3, 4, 5, 6, 7, 8};
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
18:36: variable 'x' is declared more than one times
Semantic failed. Exiting.
*/