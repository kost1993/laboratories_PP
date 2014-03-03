#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include "math_alt.h"

#define BUFSIZER 1024

int main (int argc, char **argv) {
	double i;
	double rezult1;
	double rezult2;
	double n;
	char buffer[BUFSIZER];
	//i = exp(21.48);
	n=read(0,buffer,BUFSIZER);
	sscanf(buffer,"%lf",&i);
	//printf("\n%lf\n",i);
	rezult1 = exp_alt(i);
	printf("S\t=\t%50.0lf\n",rezult1);
	rezult2 = exp(i);
	printf("M\t=\t%50.0lf\n",rezult2);
	printf("div\t=\t%50.0lf\n",rezult2-rezult1);
	rezult1 = pow_i_alt(i,100);
	printf("%50.10lf\n",rezult1);
	rezult2 = pow(i,100);
	printf("%50.10lf\n",rezult2);
	printf("div = %50.10lf\n",rezult2-rezult1);
	return 0;
}
