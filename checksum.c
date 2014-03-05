#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
/*
#include <float.h>
#include <limits.h>
*/
#include <string.h>
#include <fcntl.h> /* open(), O_RDONLY, O_WRONLY */
#include <unistd.h> /* read(), write(), close() */
#include "math_alt.h"
#include "string_alt.h"

#define BUFSIZER 1024
#define NAME_MAX 256
#define NUM_THREADS 8

pthread_mutex_t mutex;
static long long int hashsum;

void *work(void *number);

int main(int argc, char **argv)
{
	double i;
	int index_tread;
	int sum_init_treads = 0;
	int y;
	pthread_t id[NUM_THREADS];
	int data_read_bytes;
	int size_of_read_for_hash = 8;
	char *stroka;
	char filename[NAME_MAX];
	int inp_file_d;
	long long int numberhash[NUM_THREADS];
	double rezult1;
	double rezult2;
	double n;
	char buffer[BUFSIZER];
	pthread_mutex_init(&mutex, NULL);
	/*
	printf("DBL_MAX\t=\t%g\n", DBL_MAX);
	*/
	if (argc == 2) {
		strcpy(filename, argv[1]);
		inp_file_d = open(filename, O_RDONLY);
		index_tread = 0;
		do {
			memset(buffer, 0, BUFSIZER);
			data_read_bytes = read(inp_file_d, buffer,
				size_of_read_for_hash);
			numberhash[index_tread] = bytestoint(buffer);
			/*
			printf("%s\n", buffer);
			*/
			printf("%d\t%llx\n", index_tread,
				numberhash[index_tread]);
			if (index_tread < NUM_THREADS) {
				if (pthread_create(&id[index_tread],
					NULL , work,
					&numberhash[index_tread])) {
					printf("Error creating thread\n");
					exit(19);
				}
			}
			sum_init_treads++;
			index_tread++;
		} while (data_read_bytes == size_of_read_for_hash);
		close(inp_file_d);
	}
	for (index_tread = 0; index_tread < sum_init_treads; index_tread++)
		pthread_join(id[index_tread], NULL);
	/*
	n=read(0, buffer, BUFSIZER);
	sscanf(buffer,"%lf", &i);
	printf("\n%lf\n",i);
	rezult1 = exp_alt(i);
	printf("exp_alt\t=\t%g\n", rezult1);
	rezult2 = exp(i);
	printf("exp\t=\t%g\n", rezult2);
	printf("div\t=\t%g\n", rezult2 - rezult1);
	printf("%d\n", (int) sizeof(char));
	printf("%d %d\n", (int) sizeof(int), (int) sizeof(long long int));
	printf("%d %d\n", (int) sizeof(float), (int) sizeof(double));
	printf("%d %ld\n", INT_MAX, LONG_MAX);
	printf("%g %g\n", FLT_MAX, DBL_MAX);
	printf("%llx\n", 9223372036854775807);
	i = 10;
	y = 23;
	rezult1 = pow_i_alt(i,y);
	printf("%g^%d\t=\t%g\n",i,y,rezult1);
	rezult2 = pow(i,y);
	printf("%g^%d\t=\t%g\n",i,y,rezult2);
	printf("div\t=\t%g\n",rezult2-rezult1);
	*/
	return 0;
}

void *work(void *number)
{
	long long int local_number = *(long long int *) number;
	long long int exp_rezult;
	exp_rezult = exp(local_number);
	printf("exp\t=\t%lld\n", exp_rezult);
	pthread_mutex_lock(&mutex);
	hashsum ^= exp_rezult;
	printf("hash\t=\t%llx\n", hashsum);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
