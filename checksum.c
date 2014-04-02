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
#define MAX_CONS_STR 80
#define MAX_NUM_THREADS 128
#define BUFSIZE_CORES_STR 4
#define LENGTH_OF_BLOCK 8
#define NAN_ALT (0.0/0.0)

pthread_mutex_t mutex;
static unsigned long long int hashsum;
static char *buffer;
unsigned long long int read_buffer_size;
unsigned long long int count_of_blocks;
unsigned long long int *completed_blocks;
unsigned long long int *position_thread;
pthread_t *id;
int *massive_of_indexes_thread;
unsigned long long int number_blocks;
unsigned long long int modulo_number_blocks;
int ALLOWED_NUM_THREADS;

void free_mem();
void *counter(void *i);
void *work(void *number);

int main(int argc, char **argv)
{
	struct timeval start_time, end_time;
	double time_elapsed;
	int index_thread;
	int i;
	pthread_t id_progress;
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
	int scaned_numbers;
	/*int pos_i;
	char *buffer;
	char buffer_medium[LENGTH_OF_BLOCK];*/
	gettimeofday(&start_time, NULL);
	pthread_mutex_init(&mutex, NULL);
	hashsum = 0;
	ALLOWED_NUM_THREADS = 1;
	inp_file_d = open("/sys/devices/system/cpu/present", O_RDONLY);
	if (inp_file_d == -1) {
		printf("Error open file of count of cores.\n");
		return 1;
	}
	data_read_bytes = read(inp_file_d, cores_count_str, BUFSIZE_CORES_STR);
	close(inp_file_d);
	if (data_read_bytes == 2)
		cores_count = 1;
	else {
		scaned_numbers = sscanf(cores_count_str+2, "%d", &cores_count);
		if (scaned_numbers == 0) {
			printf("Error read count of cores.\n");
			return 1;
		}
		cores_count++;
	}
	printf("Present cores\t=\t%d\n", cores_count);
	ALLOWED_NUM_THREADS = cores_count * 2;
	if (ALLOWED_NUM_THREADS > MAX_NUM_THREADS)
		ALLOWED_NUM_THREADS = MAX_NUM_THREADS;
	if (argc >= 3) {
		i = 1;
		do {
			if (strcmp(argv[i], "-t") == 0 && argv[i+1] != NULL) {
				scaned_numbers = sscanf(argv[i+1], "%d",
					&ALLOWED_NUM_THREADS);
				if (scaned_numbers == 1) {
					if (ALLOWED_NUM_THREADS > MAX_NUM_THREADS) {
						printf("Custom number is bigger than %d. Using default values.\n", MAX_NUM_THREADS);
						ALLOWED_NUM_THREADS =
							cores_count * 2;
					} else {
						printf("Custom number threads: %d\n",
							ALLOWED_NUM_THREADS);
					}
				} else {
					printf("Cant recongize number for custom number threads\n");
				}
			}
			i++;
		} while (argv[i+1] != NULL);
	}
	/*ALLOWED_NUM_THREADS = 2;*/
	position_thread = malloc(sizeof(unsigned long long int) *
		ALLOWED_NUM_THREADS);
	id = malloc(sizeof(pthread_t) * ALLOWED_NUM_THREADS);
	completed_blocks = malloc(sizeof(unsigned long long int) *
		ALLOWED_NUM_THREADS);
	massive_of_indexes_thread = malloc(sizeof(int) * ALLOWED_NUM_THREADS);
	/*inp_file_d = open("/proc/meminfo", O_RDONLY);
	close(inp_file_d);*/
	memory_file = fopen("/proc/meminfo", "r");
	if (memory_file == NULL) {
		printf("Error open memory file.\n");
		return 1;
	}
	printf("Start reading memory...\n");
	fgets(memory_file_str, MAX_LEN_STR, memory_file);
	fgets(memory_file_str, MAX_LEN_STR, memory_file);
	fclose(memory_file);
	scaned_numbers = sscanf(memory_file_str, "MemFree: %llu kB",
		&free_memory);
	if (scaned_numbers == 0) {
		printf("Error find information about free memory.\n");
		free_mem();
		return 1;
	}
	printf("%s", memory_file_str);
	allowed_memory = free_memory / 2;
	allowed_memory *= 1024;
	printf("Allowed memory\t=\t%llu bytes\n", allowed_memory);
	if (argc >= 2) {
		strcpy(filename, argv[1]);
		inp_file_d = open(filename, O_RDONLY);
		if (inp_file_d == -1) {
			printf("Cant open input file:\n %s\n", filename);
			free_mem();
			return 1;
		}
		index_thread = 0;
		buffer = (char *)malloc(allowed_memory*sizeof(char));
		do {
			data_read_bytes = read(inp_file_d, buffer,
				allowed_memory);
			if (data_read_bytes == -1) {
				printf("Cant read input file (maybe it is a folder):\n %s\n", filename);
				free_mem();
				free(buffer);
				return 1;
			}
			read_buffer_size = data_read_bytes;
			position = 0;
			printf("Used memory\t=\t%lld bytes\n", data_read_bytes);
			fflush(stdout);
			number_blocks = read_buffer_size / LENGTH_OF_BLOCK;
			modulo_number_blocks = read_buffer_size %
				LENGTH_OF_BLOCK;
			if (modulo_number_blocks != 0)
				number_blocks++;
			index_thread = 0;
			/*Creating threads*/
			do {
				position_thread[index_thread] = position;
				massive_of_indexes_thread[index_thread] =
					index_thread;
				if (pthread_create(&id[index_thread],
					NULL, work,
					&massive_of_indexes_thread[index_thread])) {
					printf("Error creat thread\n");
					free_mem();
					free(buffer);
					exit(19);
				}
				index_thread++;
				position += LENGTH_OF_BLOCK;
			} while (index_thread < ALLOWED_NUM_THREADS &&
				position < data_read_bytes);
			if (pthread_create(&id_progress, NULL, counter, NULL)) {
				printf("Error creat thread\n");
				free_mem();
				free(buffer);
				exit(19);
			}
			index_thread--;
			/*Closing threads*/
			do {
				pthread_join(id[index_thread], NULL);
				index_thread--;
			} while (index_thread >= 0);
			pthread_join(id_progress, NULL);
			/*printf("Threads closed\n");*/
			fflush(stdout);
			printf("\n");
			fflush(stdout);
		} while (data_read_bytes == allowed_memory);
		close(inp_file_d);
		free(buffer);
	}
	printf("hash\t=\t%llx\n", hashsum);
	free_mem();
	gettimeofday(&end_time, NULL);
	/*sec to ms*/
	time_elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
	/*us to ms*/
	time_elapsed += (end_time.tv_usec - start_time.tv_usec) / 1000.0;
	printf("Time elapsed %g ms\n", time_elapsed);
	return 0;
}

void free_mem()
{
	free(position_thread);
	free(id);
	free(completed_blocks);
	free(massive_of_indexes_thread);
}

void *counter(void *i)
{
	char mess_progress[MAX_CONS_STR];
	int index;
	unsigned long long int sum_completed_blocks;
	int progress_number;
	do {
		memset(mess_progress, 0 , MAX_CONS_STR);
		sum_completed_blocks = 0;
		for (index = 0; index < ALLOWED_NUM_THREADS; index++)
			sum_completed_blocks += completed_blocks[index];
		progress_number = 100 * sum_completed_blocks / number_blocks;
		sprintf(mess_progress, "\rProgress:%3.0d", progress_number);
		write(1, mess_progress, sizeof(mess_progress));
		fflush(stdout);
	} while (progress_number != 100);
	pthread_exit(NULL);
}

void *work(void *number)
{
	int local_index = *(int *) number;
	unsigned long long int local_number;
	unsigned long long int llu_rezult;
	double g_ln_rezult;
	int pos_i;
	char buffer_medium[LENGTH_OF_BLOCK];
	unsigned long long int local_hashsum;
	local_hashsum = 0;
	completed_blocks[local_index] = 0;
	do {
		memset(buffer_medium, 0, LENGTH_OF_BLOCK);
		pos_i = 0;
		do {
			buffer_medium[pos_i] =
				buffer[position_thread[local_index] + pos_i];
			pos_i++;
		} while (pos_i < LENGTH_OF_BLOCK);
		local_number = bytestoint(buffer_medium);
		g_ln_rezult = ln_alt(local_number);
		if (g_ln_rezult == NAN_ALT)
			g_ln_rezult = lli_min;
		if (g_ln_rezult > 0) {
			while (g_ln_rezult * 10.0 < lli_max)
				g_ln_rezult *= 10.0;
		} else if (g_ln_rezult < 0) {
			while (g_ln_rezult * 10.0 > lli_min)
				g_ln_rezult *= 10.0;
		}
		llu_rezult = g_ln_rezult;
		local_hashsum ^= llu_rezult;
		position_thread[local_index] +=
			(LENGTH_OF_BLOCK * ALLOWED_NUM_THREADS);
		completed_blocks[local_index]++;
	} while (position_thread[local_index] < read_buffer_size);
	pthread_mutex_lock(&mutex);
	hashsum ^= local_hashsum;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
