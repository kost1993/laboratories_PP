#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*#include <math.h>*/
#include <string.h>
#include <fcntl.h> /* open(), O_RDONLY, O_WRONLY */
#include <unistd.h> /* read(), write(), close() */
#include <sys/time.h> /* gettimeofday() */
#include "math_alt.h"
#include "string_alt.h" /* bytestoint() */

#define BUFSIZER 1024
#define NAME_MAX 256
#define MAX_LEN_STR 128
#define MAX_NUM_THREADS 8
#define BUFSIZE_CORES_STR 4
#define LENGTH_OF_BLOCK 8

pthread_mutex_t mutex;
static unsigned long long int hashsum;
static char *buffer;

void *work(void *number);

int main(int argc, char **argv)
{
	struct timeval start_time, end_time;
	double time_elapsed;
	int index_tread;
	pthread_t id[MAX_NUM_THREADS];
	int ALLOWED_NUM_THREADS;
	unsigned long long int data_read_bytes;
	char filename[NAME_MAX];
	char cores_count_str[BUFSIZE_CORES_STR];
	int cores_count;
	FILE *memory_file;
	char memory_file_str[MAX_LEN_STR];
	unsigned long long int free_memory;
	unsigned long long int allowed_memory;
	unsigned long long int position;
	int inp_file_d;
	unsigned long long int numberhash[MAX_NUM_THREADS];
	int scaned_numbers;
	/*int pos_i;
	char *buffer;
	char buffer_medium[LENGTH_OF_BLOCK];*/
	gettimeofday(&start_time, NULL);
	pthread_mutex_init(&mutex, NULL);
	inp_file_d = open("/sys/devices/system/cpu/present", O_RDONLY);
	data_read_bytes = read(inp_file_d, cores_count_str, BUFSIZE_CORES_STR);
	close(inp_file_d);
	if (data_read_bytes == 2)
		cores_count = 1;
	else {
		scaned_numbers = sscanf(cores_count_str+2, "%d", &cores_count);
		cores_count++;
	}
	printf("Present cores\t=\t%d\n", cores_count);
	ALLOWED_NUM_THREADS = cores_count * 2;
	/*ALLOWED_NUM_THREADS = 4;*/
	/*inp_file_d = open("/proc/meminfo", O_RDONLY);
	close(inp_file_d);*/
	memory_file = fopen("/proc/meminfo", "r");
	printf("Start reading memory...\n");
	fgets(memory_file_str, MAX_LEN_STR, memory_file);
	fgets(memory_file_str, MAX_LEN_STR, memory_file);
	fclose(memory_file);
	scaned_numbers = sscanf(memory_file_str, "MemFree: %llu kB",
		&free_memory);
	printf("%s", memory_file_str);
	allowed_memory = free_memory / 2;
	allowed_memory *= 1024;
	printf("Allowed memory\t=\t%llu bytes\n", allowed_memory);
	if (argc == 2) {
		strcpy(filename, argv[1]);
		inp_file_d = open(filename, O_RDONLY);
		index_tread = 0;
		buffer = (char *)malloc(allowed_memory*sizeof(char));
		do {
			data_read_bytes = read(inp_file_d, buffer,
				allowed_memory);
			position = 0;
			printf("Used memory\t=\t%lld\n", data_read_bytes);
			do {
				index_tread = 0;
				do {
					/*
					memset(buffer_medium, 0,
						LENGTH_OF_BLOCK);
					pos_i = 0;*/
					/*strncpy(buffer_medium,
						buffer + position,
						LENGTH_OF_BLOCK);*/
					/*do {
						buffer_medium[pos_i] =
							buffer[position+pos_i];
						pos_i++;
					} while(pos_i < LENGTH_OF_BLOCK);
					numberhash[index_tread] =
						bytestoint(buffer_medium);*/
					numberhash[index_tread] = position;
					if (pthread_create(&id[index_tread],
						NULL , work,
						&numberhash[index_tread])) {
						printf("Error creat thread\n");
						exit(19);
					}
					index_tread++;
					position += LENGTH_OF_BLOCK;
				} while (index_tread < ALLOWED_NUM_THREADS &&
					position < data_read_bytes);
				index_tread--;
				/*printf("Closing threads\n");*/
				do {
					pthread_join(id[index_tread], NULL);
					index_tread--;
				} while (index_tread >= 0);
				/*printf("Threads closed\n");*/
			} while (position < data_read_bytes);
		} while (data_read_bytes == allowed_memory);
		close(inp_file_d);
		free(buffer);
	}
	printf("hash\t=\t%llx\n", hashsum);
	gettimeofday(&end_time, NULL);
	/*sec to ms*/
	time_elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
	/*us to ms*/
	time_elapsed += (end_time.tv_usec - start_time.tv_usec) / 1000.0;
	printf("Time elapsed %g ms\n", time_elapsed);
	return 0;
}

void *work(void *number)
{
	unsigned long long int local_number =
		*(unsigned long long int *) number;
	unsigned long long int rezult;
	double ln_rezult;
	int pos_i;
	char buffer_medium[LENGTH_OF_BLOCK];
	memset(buffer_medium, 0, LENGTH_OF_BLOCK);
	pos_i = 0;
	/*strncpy(buffer_medium, buffer + position, LENGTH_OF_BLOCK);*/
	do {
		buffer_medium[pos_i] = buffer[local_number + pos_i];
		pos_i++;
	} while (pos_i < LENGTH_OF_BLOCK);
	local_number = bytestoint(buffer_medium);
	ln_rezult = ln_alt(local_number);
	do {
		ln_rezult *= 10.0;
	} while (ln_rezult * 10.0 < lli_max);
	rezult = ln_rezult;
	pthread_mutex_lock(&mutex);
	hashsum ^= rezult;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
