
/*#include <stdio.h>*/

/*#define divergence 0.000000001*/
#define bordervid 2

double exp_alt_core(double);
double pow_d_i_alt(double, int);
double ln_alt_core(double);

double exp_alt(double x)
{
	double rezult = 0.0;
	int rank = 1;
	/* exp(x) = exp(x/y)^y */
	while ((x > bordervid) || (x < (0.0 - bordervid))) {
		x = x / 2.0;
		rank *= 2;
	}
	if (x >= 0) {
		rezult = exp_alt_core(x);
	} else {
		double invx = 0.0 - x;
		double between_rezult = 0.0;
		between_rezult = exp_alt_core(invx);
		rezult = 1.0 / between_rezult;
	}
	/*printf("x\t=\t%g\n", x);
	printf("rank\t=\t%d\n", rank);
	printf("rezult\t=\t%g\n", rezult);*/
	rezult = pow_d_i_alt(rezult, rank);
	return rezult;
}

double exp_alt_core(double x)
{
	double N = 1;
	double S = 1;
	double Sp = -1;
	double A = 1;
	double prev;
	while (Sp != S) {
		prev = Sp;
		Sp = S;
		A *= (x / N);
		S += A;
		N++;
	}
	/*printf("N\t=\t%g\n", N);
	printf("A\t=\t%g\n", A);
	printf("Pr\t=\t%g\n", prev);*/
	return S;
}

double pow_alt(double x, double y)
{
	return 0;
}

double pow_d_i_alt(double x, int y)
{
	if (y == 0)
		return 1;
	double rezult = 1;
	int i;
	for (i = 1; i <= y; i++) {
		rezult *= x;
		/*printf("%g\n",rezult);*/
	}
	return rezult;
}

long long int pow_i_i_alt(int x, int y)
{
	if (y == 0)
		return 1;
	long long int rezult = 1;
	int i;
	for (i = 1; i <= y; i++) {
		rezult *= x;
		/*
		printf("%g\n",rezult);
		*/
	}
	return rezult;
}

long long int lli_min = 0x8000000000000000;
long long int lli_max = 0x7fffffffffffffff;
const double M_E_alt = 2.71828182845904523536;

double ln_alt(double y)
{
	double rezult = 0.0;
	int rank = 0;
	/* ln(x) = ln(x/e) + 1 */
	if (y <= 0) {
		/*printf("Error ln from y < 0");*/
		return lli_min;
	}
	while (y > bordervid) {
		y = y / M_E_alt;
		rank++;
	}
	/*printf("rank\t=\t%d\n", rank);
	printf("x\t=\t%g\n", x);
	printf("rezult\t=\t%g\n", rezult);*/
	rezult = ln_alt_core(y);
	rezult += rank;
	return rezult;
}

double ln_alt_core(double y)
{
	/* ln((1+x)/(1-x)) = 2 * (x + x ^ 3 / 3 + x ^ 5 / 5 + x ^ 7 / 7+ ...) */
	double x;
	double A;
	double Sp = -1.0;
	double S = 0.0;
	double N = 1.0;
	lli_min += 1;
	x = (y - 1.0) / (y + 1.0);
	/*printf("y\t=\t%.12g\n", y);
	printf("x\t=\t%g\n", x);*/
	A = x;
	while (Sp != S) {
		Sp = S;
		S += (A / N);
		A *= (x * x);
		N += 2.0;
		/*printf("N\t=\t%g\n", N);
		printf("A\t=\t%g\n", A);
		printf("Sp\t=\t%g\n", Sp);*/
	}
	/*printf("N\t=\t%g\n", N);*/
	return 2*S;
}
