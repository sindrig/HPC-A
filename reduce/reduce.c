#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

void cardgame(int *, int *, int *, MPI_Datatype *);

void cardgame(int *inarr, int *inout, int *length, MPI_Datatype *type){
	inout[0] = abs(inout[0] - inarr[0]);
}


int main(int argc, char**argv){
	int rank, size, rnum, prod, sum, handval;
	MPI_Op my_MPI;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Op_create((MPI_User_function *) cardgame, 1, &my_MPI);

	// Initial seed
	srand(time(NULL) + rank);

	rnum = 1 + rand() / (RAND_MAX / 14 + 1);
	printf("Task %d: I got %d\n", rank, rnum);

	MPI_Reduce(&rnum, &prod, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);
	MPI_Reduce(&rnum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&rnum, &handval, 1, MPI_INT, my_MPI, 0, MPI_COMM_WORLD);

	if (rank == 0){
		sleep(1);
		printf("All numbers gathered, the sum is %d and the product is %d\n", sum, prod);
		printf("Also, we played a bogus game with your numbers and got the value %d\n", handval);
	}
	

	MPI_Finalize();
	return 0;
}

