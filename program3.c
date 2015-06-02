#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/****************************************************************/
/* comment the line below to stop verbose fixed function output */
/****************************************************************/
//#define FP_DEBUG



/* let's just slap our numbers in an unsigned int */
typedef unsigned int fixed;

/*
 * Scaling and descaling really just means shifting half the bits.
 * When we need to use the square root of the scalar, we really
 * can just shift 1/4 of the bits.
 */
const unsigned int SCALAR_BITS = sizeof(fixed) * 4;
const unsigned int SQRT_SCALAR_BITS = sizeof(fixed) * 2;

fixed int_to_fixed(unsigned int n);
unsigned int fixed_real_part(fixed f);
unsigned int fixed_whole_part(fixed f);
fixed fixed_add(fixed x, fixed y);
fixed fixed_sub(fixed x, fixed y);
fixed fixed_mult(fixed x, fixed y);
fixed fixed_div(fixed x, fixed y);
void fixed_print(fixed f);

fixed int_to_fixed(unsigned int n) {
	fixed result = (fixed)(n << SCALAR_BITS);
#ifdef FP_DEBUG
	printf("int_to_fixed() result: ");
	fixed_print(result);
	printf("\n");
#endif /* FP_DEBUG */
	return result;
}

unsigned int fixed_real_part(fixed f) {
	/* just zero out the high half of the bits */
	return (unsigned int)(f & ~((~(fixed)0) << SCALAR_BITS));
}

unsigned int fixed_whole_part(fixed f) {
	/* 
	 * Hmm... we can't just zero out the low bits because the result
	 * would be be too big... scaled in fact
	 */
	return (unsigned int)(f >> SCALAR_BITS);
}

fixed fixed_add(fixed x, fixed y) {
	fixed result = x + y;
#ifdef FP_DEBUG
	printf("fixed_add() result: ");
	fixed_print(result);
	printf("\n");
#endif /* FP_DEBUG */
	return result;
}

fixed fixed_sub(fixed x, fixed y) {
	fixed result = x - y;
	if(x < y) {
		printf("\nwatch out! %d - %d will overflow\n", x, y);
	}
	
#ifdef FP_DEBUG
	printf("fixed_sub() result: ");
	fixed_print(result);
	printf("\n");
#endif /* FP_DEBUG */
	return result;
}

fixed fixed_div(fixed x, fixed y) {
	fixed result;
	
	result = (x << SQRT_SCALAR_BITS) / (y >> SQRT_SCALAR_BITS);
	
#ifdef FP_DEBUG
	printf("fixed_div() result: ");
	fixed_print(result);
	printf("\n");
#endif /* FP_DEBUG */
	return result;
}

fixed fixed_mult(fixed x, fixed y) {
	/* 
	 * x = a.b and y = c.d
	 * S = scalar and Q = sqrt(S)
	 *
	 * A = a.0, B = 0.b, C = c.0, and D = 0.d
	 * 
	 * so...
	 * x * y = (A / Q) * (C / Q)
			 + (B * (C / Q)) / Q
	         + (D * (A / Q)) / Q
	         + (B * D) / S
	 */
	fixed result = (fixed)0;
	fixed A_Q = (x & (~(fixed)0 << SCALAR_BITS)) >> SQRT_SCALAR_BITS;
	fixed C_Q = (y & (~(fixed)0 << SCALAR_BITS)) >> SQRT_SCALAR_BITS;
	fixed B   = x & (~(fixed)0 >> SCALAR_BITS);
	fixed D   = y & (~(fixed)0 >> SCALAR_BITS);
	
	result += A_Q * C_Q;
	result += (B * C_Q) >> SQRT_SCALAR_BITS;
	result += (D * A_Q) >> SQRT_SCALAR_BITS;
	result += (B * D) >> SCALAR_BITS;
	
#ifdef FP_DEBUG
	printf("fixed_mult() result: ");
	fixed_print(result);
	printf("\n");
#endif /* FP_DEBUG */
	return result;
}

void fixed_print(fixed f) {
	unsigned int whole = fixed_whole_part(f);
	unsigned int real = fixed_real_part(f);
	unsigned int real_denominator = 1 << SCALAR_BITS;
	
	/* 
	 * find the least significant bit with a 1 in the real part.
	 * Then we can right shift away all those low zeros in the 
	 * real and and denominator.
	 */
	int i = 0;
	while(((real & (1 << 0)) == 0) && (i < SCALAR_BITS)) {
		real = real >> 1;
		real_denominator = real_denominator >> 1;
		i++;
	}
	
	printf("%d", whole);
	if(real > 0) {
		printf(" + %d/%d", real, real_denominator);
	}
	
}


int main(int argc, const char * argv[])
{
	fixed Dpart, Cpart, D, C, pounds, wpounds, rpounds, pcost, dividen, fcash,
	total, realp, change;
	int i, cash;
	dividen = int_to_fixed(100);
	char *rp = malloc(8*sizeof(char));
	printf("What is the pound-cost of bananas?\n");
	printf("\nDollar part first: ");
	scanf("%d", &Dpart);
	D = int_to_fixed(Dpart);
	printf("\nCent part next: ");
	scanf("%d", &Cpart);
	C = int_to_fixed(Cpart);
	C = fixed_div(C, dividen);
	pcost = fixed_add(D, C);
	printf("\n...so bananas are ");
	fixed_print(pcost);
	printf(" Dollars per pound.\n");


	printf("\nHow many whole pounds: ");
	scanf("%d", &wpounds);
	wpounds = int_to_fixed(wpounds);
	printf("\nHow many real pounds (imagine a 0. first): ");
	scanf("%s", rp); //Used a string to get leading zeros ex. .05 pounds.
	realp = atoi(rp);
	rpounds = int_to_fixed(realp);
	rpounds = fixed_div(rpounds, dividen);
	pounds = fixed_add(wpounds, rpounds);
	printf("\n...so you're getting ");
	fixed_print(pounds);
	printf(" pounds.\n");

	printf("\nThat will cost ");
	total = fixed_mult(pcost, pounds);
	fixed_print(total);
	printf(" dollars.\n");

	printf("\nHow many whole dollars will you give me?: ");
	scanf("%d", &cash);
	fcash = int_to_fixed(cash);
	change = fixed_sub(fcash, total);

	printf("\nYour change is ");
	fixed_print(change);
	printf(" dollars.\n");

	free(rp);
    return 0;
}
