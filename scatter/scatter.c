#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(int argc, char**argv){
	// Calculate the sum of of squares from 1 to n
    int rank, size, n, i, send_count;
    //Default
    n = 5;
    if(argc > 1){
        n = atoi(argv[1]);
    }

    if(n < 2){
        printf("n needs to be larger than 1\n");
        return 1;
    }


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    send_count = (n + (size - 1)) / size;
    
    int arr[n];
    if(rank == 0){
        for(i = 0; i < n; i++){
            arr[i] = i+1;
        }
        printf("n=%d, send_count=%d, size=%d\n", n, send_count, size); 
    }

    int *recv_buf = malloc(sizeof(int) * send_count);

    MPI_Scatter(arr, send_count, MPI_INT, recv_buf, send_count, MPI_INT, 0, MPI_COMM_WORLD);

    int squared[send_count];
    for(i = 0; i < send_count; i++){
        if((rank+1)*send_count+i-1 > n){
            // Some processes might not get any elements. For this we will have to use MPI_Scatterv
            break;
        }
        squared[i] = recv_buf[i] * recv_buf[i];
    }

    int *squared_all = NULL;
    if(rank == 0){
        squared_all = malloc(sizeof(int) * size * send_count);
    }

    MPI_Gather(&squared, send_count, MPI_INT, squared_all, send_count, MPI_INT, 0, MPI_COMM_WORLD);

    int res = 0;
    if(rank == 0){
        for(i = 0; i < n; i++){
            res += squared_all[i];
        }
        printf("Result: %d\n", res);
    }

	MPI_Finalize();
	return 0;
}

