#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void printRandoms(int upper, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        int num = rand();
        int c = 0;
        while ((num > upper) || (num < 0))
        {
            c += rand();
            if (num < 0)
            {
                num = (rand() % upper + 1);
            }
            else
            {
                num = num - (pow(2, c));
            }
        }
        printf("\n===%d", num);
    }
}

// Driver code
int main()
{
    int upper = 999, count = 10;

    // Use current time as seed for random generator
    srand(rand());

    printRandoms(upper, count);

    return 0;
}