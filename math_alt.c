/*
#include <stdio.h>
*/

/*
#define divergence 0.000000001
*/
#define bordervid 2

double exp_alt_core(double);
double pow_d_i_alt(double, int);

double exp_alt(double x)
{
	double rezult = 0.0;
	int rank = 1;
	/*
	exp(x) = exp(x/y)^y
	*/
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
	/*
	printf("x\t=\t%g\n",x);
	printf("rank\t=\t%d\n",rank);
	printf("rezult\t=\t%g\n",rezult);
	*/
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
	/*
	printf("N\t=\t%g\n",N);
	printf("A\t=\t%g\n",A);
	printf("Pr\t=\t%g\n",prev);
	*/
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
		/*
		printf("%g\n",rezult);
		*/
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
