#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <fcntl.h> /* open(), O_RDONLY, O_WRONLY */
#include <unistd.h> /* read(), write(), close() */
#include <sys/stat.h> /* struct stat */
#include <mpi.h>
#include <omp.h>

#define NAME_MAX 256
#define MAX_LEN_STR 128
#define LENGTH_OF_BLOCK sizeof(unsigned long long int)
#define bit_in_byte 8
#define SIZE_LLU sizeof(unsigned long long int)
#define NAN_ALT (0.0/0.0)
#define bordervid 2

unsigned long long int *buffer_llu;
unsigned long long int number_blocks;
int inp_source_d;

unsigned long long int work(unsigned long long int pos, unsigned long long int pos_end);

void main(int argc, char **argv)
{
	int index_thread;
	int i;
	char filename[NAME_MAX];
	FILE *memory_file;
	char memory_file_str[MAX_LEN_STR];
	unsigned long long int free_memory;
	unsigned long long int position;
	unsigned long long int file_size;
	unsigned long long int diff_length;
	unsigned long long int diff_blocks;
	unsigned long long int part_hash;
	unsigned long long int comm_offset_check;
	unsigned long long int comm_offset_start;
	unsigned long long int size_buffer_llu;
	unsigned long long int data_read_bytes;
	int scaned_numbers;
	struct stat statfile;
	int size_comm;
	int rank_comm;
	unsigned long long int pos_start_comm;
	unsigned long long int pos_end_comm;
	int type = 99;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size_comm);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_comm);
	if (argc < 2) {
		printf("Error, need filename\n");
		exit(1);
	}
	strcpy(filename, argv[1]);
	inp_source_d = open(filename, O_RDONLY);
	if (inp_source_d == -1) {
		printf("Cant open input file:\n %s\n", filename);
		exit(1);
	}
	if (rank_comm == 0) {
		static unsigned long long int hashsum;
		hashsum = 0;
		memory_file = fopen("/proc/meminfo", "r");
		if (memory_file == NULL) {
			printf("Error open memory file.\n");
			exit(1);
		}
		printf("Start reading memory...\n");
		fgets(memory_file_str, MAX_LEN_STR, memory_file);
		fgets(memory_file_str, MAX_LEN_STR, memory_file);
		fclose(memory_file);
		scaned_numbers = sscanf(memory_file_str, "MemFree: %llu kB",
			&free_memory);
		if (scaned_numbers == 0) {
			printf("Error find information about free memory.\n");
			exit(1);
		}
		free_memory *= 1024;
		printf("%s", memory_file_str);

		fstat(inp_source_d, &statfile);
		file_size = (unsigned long long)statfile.st_size;
		if (file_size > free_memory) {
			printf("PLease use file with size < %llu", free_memory);
			close(inp_source_d);
			exit(1);
		}
		number_blocks = ceil(file_size / (double)LENGTH_OF_BLOCK);
		position = 0;
		diff_blocks = number_blocks;
		unsigned long long int *position_thread;
		unsigned long long int *position_thread_end;
		position_thread = malloc(SIZE_LLU * size_comm);
		position_thread_end = malloc(SIZE_LLU * size_comm);
		for (index_thread = 0; index_thread < size_comm; index_thread++) {
			position_thread[index_thread] = position;
			diff_length = ceil(diff_blocks / (double)(size_comm - index_thread));
			if (index_thread == size_comm - 1)
				position_thread_end[index_thread] = number_blocks;
			else
				position_thread_end[index_thread] = position + diff_length;
			diff_blocks -= diff_length;
			position = position_thread_end[index_thread];
		}
		for (i = 1; i < size_comm; i++) {
			MPI_Send(&position_thread[i], SIZE_LLU, MPI_UNSIGNED_LONG_LONG, i, type, MPI_COMM_WORLD);
			MPI_Send(&position_thread_end[i], SIZE_LLU, MPI_UNSIGNED_LONG_LONG, i, type, MPI_COMM_WORLD);
		}
		pos_start_comm = position_thread[0];
		pos_end_comm = position_thread_end[0];
		comm_offset_start = pos_start_comm * LENGTH_OF_BLOCK;
		comm_offset_check = lseek(inp_source_d, comm_offset_start, 0);
		size_buffer_llu = (pos_end_comm - pos_start_comm) * SIZE_LLU;
		buffer_llu = (unsigned long long int *)malloc(size_buffer_llu);
		data_read_bytes = read(inp_source_d, buffer_llu, size_buffer_llu);
		part_hash = work(pos_start_comm, pos_end_comm);
		free(buffer_llu);
		hashsum = part_hash;
		for (i = 1; i < size_comm; i++) {
			MPI_Recv(&part_hash, SIZE_LLU, MPI_UNSIGNED_LONG_LONG, i, type, MPI_COMM_WORLD, &status);
			hashsum = hashsum ^ part_hash;
		}
		printf("hash\t=\t%16llx\n", hashsum);
		free(position_thread);
		free(position_thread_end);
	} else {
		MPI_Recv(&pos_start_comm, SIZE_LLU, MPI_UNSIGNED_LONG_LONG, 0, type, MPI_COMM_WORLD, &status);
		MPI_Recv(&pos_end_comm, SIZE_LLU, MPI_UNSIGNED_LONG_LONG, 0, type, MPI_COMM_WORLD, &status);
		comm_offset_start = pos_start_comm * LENGTH_OF_BLOCK;
		comm_offset_check = lseek(inp_source_d, comm_offset_start, 0);
		size_buffer_llu = (pos_end_comm - pos_start_comm) * SIZE_LLU;
		buffer_llu = (unsigned long long int *)malloc(size_buffer_llu);
		data_read_bytes = read(inp_source_d, buffer_llu, size_buffer_llu);
		part_hash = work(pos_start_comm, pos_end_comm);
		free(buffer_llu);
		MPI_Send(&part_hash, SIZE_LLU, MPI_UNSIGNED_LONG_LONG, 0, type, MPI_COMM_WORLD);
	}
	close(inp_source_d);
	MPI_Finalize();
}

const long long int lli_min = 0x8000000000000000;
const long long int lli_max = 0x7fffffffffffffff;
const double M_E_alt = 2.71828182845904523536;

double ln_alt_core(double y)
{
	/* ln((1+x)/(1-x)) = 2 * (x + x ^ 3 / 3 + x ^ 5 / 5 + x ^ 7 / 7+ ...) */
	double x;
	double A;
	double Sp = -1.0;
	double S = 0.0;
	double N = 1.0;
	x = (y - 1.0) / (y + 1.0);
	A = x;
	while (Sp != S) {
		Sp = S;
		S += (A / N);
		A *= (x * x);
		N += 2.0;
	}
	return 2*S;
}

double ln_alt(double y)
{
	double rezult = 0.0;
	int rank = 0;
	/* ln(x) = ln(x/e) + 1 */
	if (y <= 0) {
		return NAN_ALT;
	}
	while (y > bordervid) {
		y = y / M_E_alt;
		rank++;
	}
	rezult = ln_alt_core(y);
	rezult += rank;
	return rezult;
}

unsigned long long int work(unsigned long long int pos, unsigned long long int pos_end)
{
	unsigned long long int local_number;
	unsigned long long int llu_rezult;
	double g_ln_rezult;
	unsigned long long int local_hashsum;
	local_hashsum = 0;
	pos_end -= pos;	
	for (pos = 0; pos < pos_end; pos++) {
		local_number = buffer_llu[pos];
		int i;
		unsigned long long int rez_corr = 0;
		unsigned long long int temp_corr;
		unsigned char loop_corr;
		for (i = 1; i <= LENGTH_OF_BLOCK; i++) {
			loop_corr = local_number;
			local_number = local_number >> bit_in_byte;
			temp_corr = loop_corr;
			temp_corr = temp_corr << ((LENGTH_OF_BLOCK - i) * bit_in_byte);
			rez_corr += temp_corr;
		}
		local_number = rez_corr;
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
	}
	return local_hashsum;
}
