//mpirun -np 16 ./first.o
#include <stdio.h>
#include <mpi.h>
#include <stddef.h>

#include "fish_model.h"

#define WORLD_HEIGHT 200
#define WORLD_WIDTH 200
#define X 2
#define Y 2
#define POPULATION 2
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define ITERATIONS 100

MPI_Datatype mpi_fish_group;
void create_fish_group_datatype();

int main(int argc, char** argv)
{
    int numtasks, worldrank, rank, source, dest, outbuf, i, tag=1;
    // int inbuf[4]={MPI_PROC_NULL, MPI_PROC_NULL, MPI_PROC_NULL, MPI_PROC_NULL};
    int nbrs[4], coords[2];
    int dims[2], periods[2]={0, 0}, reorder=0;

    int target_coords[2];
    int cart_coords[2];
    int target_rank;

    dims[0] = X;
    dims[1] = Y;

    // Two arrays of fish, one for the whole (only used in initialization
    // and the other to hold each process's groups)
    fish_group groups[POPULATION];
    fish_group my_groups[POPULATION];
    int num_fish_in_cell = 0;

    MPI_Comm cartcomm;

    MPI_Request recvreqs[POPULATION];
    MPI_Status recvstats[POPULATION];

    MPI_Request sendreqs[POPULATION];
    MPI_Status sendstats[POPULATION];

    // createDimensions(dims, SIZE);

    MPI_Init(&argc, &argv);

    create_fish_group_datatype();

    // Get rank and size of the world
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldrank);

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cartcomm);
    MPI_Comm_rank(cartcomm, &rank);

    MPI_Cart_coords(cartcomm, rank, 2, coords);

    // int MPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source,
    //                    int *rank_dest)
    // Get our neighbours
    MPI_Cart_shift(cartcomm, 0, 1, &nbrs[UP], &nbrs[DOWN]);
    MPI_Cart_shift(cartcomm, 1, 1, &nbrs[LEFT], &nbrs[RIGHT]);

    if(worldrank == 0){
        // Populate the world
        printf("World: %dx%d (size:%dx%d)\n", dims[0], dims[1], WORLD_HEIGHT, WORLD_WIDTH);
        populate(groups, POPULATION, WORLD_WIDTH, WORLD_HEIGHT);
        for(i = 0; i < POPULATION; i++){
            printf("fish: %d, x=%d, y=%d\n", groups[i].num, groups[i].x, groups[i].y);
        }
    }

    // Broadcast all fish groups to all cells
    MPI_Bcast(groups, POPULATION, mpi_fish_group, 0, cartcomm);

    // Each cell has to figure out which fish groups belongs to it
    for(i = 0; i < POPULATION; i++){
        int cart_coords[2];
        get_cart_coords(cart_coords, groups[i], WORLD_WIDTH, WORLD_HEIGHT, dims[0], dims[1]);
        if(cart_coords[0] == coords[0] && cart_coords[1] == coords[1]){
            // This fish group belongs to this cell
            my_groups[num_fish_in_cell++] = groups[i];
        }
    }

    update(my_groups, num_fish_in_cell);
    for(i = 0; i < num_fish_in_cell; i++){
        get_cart_coords(cart_coords, groups[i], WORLD_WIDTH, WORLD_HEIGHT, dims[0], dims[1]);
        MPI_Cart_rank(cartcomm, cart_coords, &target_rank);
        printf("Rank %d has group with %d fish in it\n", rank, my_groups[i].num);
        printf("fish: %d, x=%d, y=%d\n", my_groups[i].num, my_groups[i].x, my_groups[i].y);
        if(rank != target_rank){
            printf("Should be sent to %d\n", target_rank);
            // MPI_Isend(my_groups[i], 1, mpi_fish_group, target_rank,
            //     tag, cartcomm, &sendreqs[i])
        }
    }

    printf("rank = %d, coords = %d,%d, having neighbours(u,d,l,r) = %d %d %d %d \n",
            rank, coords[0], coords[1], nbrs[UP], nbrs[DOWN], nbrs[LEFT], nbrs[RIGHT]);

    MPI_Finalize();
    return 0;
}

void create_fish_group_datatype(){
    // Set up parameters to create our mpi datatype
    int nitems = 5;
    int blocklengths[5] = {1, 2, 3, 4, 5};
    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offsets[5];

    offsets[0] = offsetof(fish_group, num);
    offsets[1] = offsetof(fish_group, x);
    offsets[2] = offsetof(fish_group, y);
    offsets[3] = offsetof(fish_group, x_speed);
    offsets[4] = offsetof(fish_group, y_speed);

    // Create our mpi datatype
    MPI_Type_create_struct(5, blocklengths, offsets, types, &mpi_fish_group);
    MPI_Type_commit(&mpi_fish_group);
}
