#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
main(int argc, char **argv)
{
	char message[20];
	int  i, rank, size, type = 99;
	int buf_number;
	MPI_Status status;

	buf_number = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	srand(time(NULL) + rank);
	if (rank == 0) {
		/*strcpy(message, "Hello, world!");*/
		for (i = 1; i < size; i++) {
			buf_number = rand()%65000;
			/*MPI_Send(message, 14, MPI_CHAR, i, type, MPI_COMM_WORLD);*/
			MPI_Send(&buf_number, sizeof(int), MPI_INT, i, type, MPI_COMM_WORLD);
			printf("Master\t%3d\tsend\t%d\tto\tprocess %3d\n", rank, buf_number, i);
		}
		for (i = 1; i < size; i++) {
			/*MPI_Send(message, 14, MPI_CHAR, i, type, MPI_COMM_WORLD);*/
			MPI_Recv(&buf_number, sizeof(int), MPI_INT, i, type, MPI_COMM_WORLD, &status);
			printf("Master\t%3d\trecieve\t%d\tfrom\tprocess %3d\n", rank, buf_number, i);
		}
	} else {
		/*MPI_Recv(message, 20, MPI_CHAR, 0, type, MPI_COMM_WORLD, &status);*/
		MPI_Recv(&buf_number, sizeof(int), MPI_INT, 0, type, MPI_COMM_WORLD, &status);
		printf("Process\t%3d\trecieve\t%d\tfrom\tMaster\n", rank, buf_number);
		buf_number = rand()%65000;
		MPI_Send(&buf_number, sizeof(int), MPI_INT, 0, type, MPI_COMM_WORLD);
		printf("Process\t%3d\tsend\t%d\tto\tMaster\n", rank, buf_number);
	}
	MPI_Finalize();
}
