//mpirun -np 16 ./first.o
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <mpi.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "fish_model.h"
#include "fish_utils.h"

#define WORLD_HEIGHT 200
#define WORLD_WIDTH 200
#define X 2
#define Y 2
#define POPULATION 2
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define OUTPUT 1

#define ITERATIONS 200

MPI_Datatype mpi_fish_group;
void create_fish_group_datatype();

int main(int argc, char** argv)
{
    int numtasks, worldrank, rank, i, tag=1;
    // int inbuf[4]={MPI_PROC_NULL, MPI_PROC_NULL, MPI_PROC_NULL, MPI_PROC_NULL};
    int nbrs[4], coords[2];
    int dims[2], periods[2]={0, 0}, reorder=0;

    int cart_coords[2];
    int target_rank;

    dims[0] = X;
    dims[1] = Y;

    // Two arrays of fish, one for the whole (only used in initialization
    // and the other to hold each process's groups)
    fish_group my_groups[POPULATION];
    int num_fish_in_cell = 0;

    MPI_Comm cartcomm;

    // MPI_Request recvreqs[POPULATION];
    // MPI_Status recvstats[POPULATION];

    // MPI_Request sendreqs[POPULATION];
    // MPI_Status sendstats[POPULATION];

    // We need at least one for each neigbour and additionally maximum
    // of POPULATION for our sends.
    // MPI_Request requests[4];
    // MPI_Status status[4];

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
        populate(my_groups, POPULATION, WORLD_WIDTH, WORLD_HEIGHT);
        num_fish_in_cell = POPULATION;
        for(i = 0; i < POPULATION; i++){
            printf("fish: %d, x=%d, y=%d\n", my_groups[i].num, my_groups[i].x, my_groups[i].y);
        }
    }

    int j = 0;

    fish_group received[POPULATION];
    MPI_Request recv_request;

    MPI_Irecv(received, POPULATION, mpi_fish_group,
        MPI_ANY_SOURCE, tag, cartcomm, &recv_request);
    while(j++ < ITERATIONS){
        int sends = 0;
        int testdone, probedone;

        MPI_Request requests[numtasks];
        MPI_Status status[numtasks];

        MPI_Status recv_status, probe_status;


        int send_counts[numtasks];
        for(i = 0; i < numtasks; i++){
            send_counts[i] = 0;
        }

        fish_group send_objects[numtasks][num_fish_in_cell];

        for(i = 0; i < num_fish_in_cell; i++){
            get_cart_coords(cart_coords, &my_groups[i], WORLD_WIDTH, WORLD_HEIGHT, dims[0], dims[1]);
            MPI_Cart_rank(cartcomm, cart_coords, &target_rank);
            // printf("Rank %d has group with %d fish in it\n", rank, my_groups[i].num);
            // printf("fish: %d, x=%d, y=%d\n", my_groups[i].num, my_groups[i].x, my_groups[i].y);
            if(rank != target_rank){
                // printf("Should be sent to %d\n", target_rank);
                int sc = send_counts[target_rank];
                send_counts[target_rank] += 1;
                // printf("send count is %d\n", sc);
                send_objects[target_rank][sc] = my_groups[i];
                // MPI_Isend(&my_groups[i], 1, mpi_fish_group, target_rank,
                //     tag, cartcomm, &requests[sends]);
                // printf("removing element\n");
                remove_element(my_groups, i, num_fish_in_cell--);
                // printf("removing element done\n");
                i--;
                // sends++;
            }
        }

        for(i = 0; i < numtasks; i++){
            if(send_counts[i] > 0){
                printf("Rank %d: Sending %d groups to %d in loop %d\n", rank, send_counts[i], i, j);
                // MPI_Isend(send_objects[i], send_counts[i], mpi_fish_group,
                //     i, tag, cartcomm, &requests[sends++]);
                MPI_Send(send_objects[i], send_counts[i], mpi_fish_group,
                    i, tag, cartcomm);
            }
        }
        MPI_Barrier(cartcomm);
        struct timespec tim;
        tim.tv_sec  = 0;
        // tim.tv_nsec = 500000000L;
        tim.tv_nsec = 50000000L;
        nanosleep(&tim, NULL);


        // printf("rank = %d, 4 + num_fish_in_cell,: %d\n", rank, 4 + num_fish_in_cell);
        if(sends > 0){
            MPI_Waitall(sends, requests, status);
        }


        MPI_Iprobe(MPI_ANY_SOURCE, tag, cartcomm, &probedone, &probe_status);

        MPI_Test(&recv_request, &testdone, &recv_status);

        int count = 0;
        while(testdone){
            MPI_Get_count(&recv_status, mpi_fish_group, &count);
            for(i = 0; i < count; i++){
                my_groups[num_fish_in_cell++] =received[i];
            }
            MPI_Irecv(received, POPULATION, mpi_fish_group,
                MPI_ANY_SOURCE, tag, cartcomm, &recv_request);

            MPI_Iprobe(MPI_ANY_SOURCE, tag, cartcomm, &probedone, &probe_status);

            MPI_Test(&recv_request, &testdone, &recv_status);
        }

        // printf("%d-%d: testdone: %d - probedone: %d - received: %d\n", rank, j, testdone, probedone, count);


        if(OUTPUT){
            // MPI_Barrier(cartcomm);
            // if(rank==0){
            //     printf("--stat\n");
            // }
            // MPI_Barrier(cartcomm);
            for(i=0; i < num_fish_in_cell; i++){
                printf("--fish-%d-%d-%d\n", my_groups[i].num, my_groups[i].x, my_groups[i].y);
            }
            // MPI_Barrier(cartcomm);
            // if(rank==0){
            //     printf("--endstat\n");
            // }
            // MPI_Barrier(cartcomm);
        }

        // Update the x,y position of every group according to it's movement speed.
        update(my_groups, num_fish_in_cell);
    }


    // printf("rank = %d, coords = %d,%d, having neighbours(u,d,l,r) = %d %d %d %d \n",
    //         rank, coords[0], coords[1], nbrs[UP], nbrs[DOWN], nbrs[LEFT], nbrs[RIGHT]);

    MPI_Finalize();
    return 0;
}

void create_fish_group_datatype(){
    // Set up parameters to create our mpi datatype
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
