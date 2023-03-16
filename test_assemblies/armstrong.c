// C program to find Armstrong number

#include <stdio.h>

/* Function to calculate x raised to the power y */
int power(int x, unsigned int y)
{
    int ret;
    int p;
    
	if (y == 0)
		return 1;
		
	p = power(x, y /2);
	if (y % 2 == 0) 
		ret = p * p;
    
	else	
	   ret = x * p * p;

    return ret;
}

/* Function to calculate order of the number */
int order(int x)
{
	int n = 0;
	while (x) {
		n++;
		x = x / 10;
	}
	return n;
}

// Function to check whether the given number is
// Armstrong number or not

int isArmstrong(long int n)
{
	long int i, sum, tem, temp;
	volatile int div = 10;
	i = sum = 0;
	tem = temp = n;

	while (n) {
		n = n / div;  
		++i;
	}

	while (tem) {
		sum += power(tem % div, i);
		tem /= div;
	}

	if (temp == sum)
		return 1;
	return 0;

}

// Driver Program
int main()
{
    int j;
	int x = 153;
	if (isArmstrong(x) == 1)
		j=printf("True\n");
	else
		j=printf("False\n");

	x = 1253;
	if (isArmstrong(x) == 1)
		j=printf("True\n");
	else
		j=printf("False\n");

	return 0;
}