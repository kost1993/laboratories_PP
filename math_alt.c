#include <stdio.h>

#define divergence 0.000000001

double exp_alt (double x) {
	double N = 1;
	double S = 1;
	double Sp=-1;
	double A = 1;
	double prev;
	//while (A > divergence) {
	while (Sp != S) {
		prev = Sp;
		Sp = S;
		A *= (x / N);
		S += A;
		N++;
	} 
	printf("N=%lf\nA\t=\t%50.10lf\nPr\t=\t%50.10lf\n",N,A,prev);
	return S;
}

double exp_alt_core (double x) {
	double N = 1;
	double S = 1;
	double Sp=-1;
	double A = 1;
	double prev;
	//while (A > divergence) {
	while (Sp != S) {
		prev = Sp;
		Sp = S;
		A *= (x / N);
		S += A;
		N++;
	} 
	printf("N=%lf\nA =%50.10lf\nPr=%50.10lf\n",N,A,prev);
	return S;
}

double pow_alt (double x,double y) {
	return 0;
}

double pow_i_alt (double x,int y) {
	if (y == 0)
		return 1;
	double rezult=1;
	int i;
	for (i = 1; i <= y; i++)
		rezult*=x;
	return rezult;
}
