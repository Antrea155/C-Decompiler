int main()
{
    int j;
	int x = 153;
	if (test(x) == 1)
		j=printf("True\n");
	else
		j=printf("False\n");

	x = 1253;
	if (test(x) == 1)
		j=printf("True\n");
	else
		j=printf("False\n");

	return 0;
}