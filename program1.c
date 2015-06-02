#include <stdio.h>

int main ()
{
int base, number, total, array[10], binarr[31];
int i=0, r=0;
printf("Enter a base: ");
scanf("%d", &base);
printf("Enter a base-%d number: ", base);
scanf("%d", &number);
while (number > 0)
{
	array[i] = number % 10;
	number = number / 10;
	i++;
}
total = 0;
while (i > -1)
{
	total = total*base+array[i];
	i--;
}
printf("Decimal: %d\n",total);
while (total > 1)
{
	binarr[r] = total % 2;
	total = total / 2;
	r++;
}
if (total==1)
	binarr[r]=1;
printf("Binary: ");
i=0;
while (r>-1)
{
	printf("%d",binarr[r]);
	r--;
}
printf("\n");
}