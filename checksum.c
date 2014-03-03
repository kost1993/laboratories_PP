#include <stdio.h>
#include <pthread.h>
#include "math_alt.h"

int main(int argc, char **argv) {
	double i;
	i = exp(10.5);
	printf("%d",i);
	return 0;
}
