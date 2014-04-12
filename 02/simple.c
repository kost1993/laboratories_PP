#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> /* gettimeofday() */
#include <math.h>

#define true 1
#define false 0

unsigned long long int simple(unsigned long long int x);
unsigned long long int divisor;


int main(int argc, char **argv)
{
	struct timeval start_time, end_time;
	double time_elapsed;
	int i;
	unsigned long long int x;
	int scaned_items;
	gettimeofday(&start_time, NULL);
	divisor = 0;
	if (argc == 2) {
		scaned_items = sscanf(argv[1], "%llu", &x);
		if (scaned_items == 0) {
			printf("Input number isn't integer.\n");
			return 1;
		}
		printf("Number of processors\t=\t%d\n", omp_get_num_procs());
		printf("Number of threads\t=\t%d\n", omp_get_max_threads());
		/*x=4000000001;*/
		if (simple(x))
			printf("Number %llu - simple\n", x);
		else {
			printf("Number %llu - not simple\n", x);
			printf("Divisor is %llu\n", divisor);
		}
	}
	gettimeofday(&end_time, NULL);
	/*sec to ms*/
	time_elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
	/*us to ms*/
	time_elapsed += (end_time.tv_usec - start_time.tv_usec) / 1000.0;
	printf("Time elapsed %g ms\n", time_elapsed);
	return 0;
}

unsigned long long int simple(unsigned long long int x)
{
	unsigned long long int i;
	unsigned long long int top;
	top = sqrt(x);
	if (x < 2)
		return false;
	if (x == 2)
		return true;
	if (x % 2 == 0) {
		divisor = 2;
		return false;
	}
	# pragma omp parallel shared(x, top)
	{
		# pragma omp for
		for (i = 3; i <= top; i += 2) {
			if (x % i == 0)
				divisor = i;
		}
	}
	if (divisor != 0)
		return false;
	else
		return true;
}
