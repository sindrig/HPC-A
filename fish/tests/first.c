//mpirun -np 16 ./first.o
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <mpi.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "fish_model.h"
#include "net_model.h"
#include "fish_utils.h"

#define WORLD_HEIGHT 200
#define WORLD_WIDTH 200
#define X 2
#define Y 2
#define POPULATION 8
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define OUTPUT 1

#define ITERATIONS 200

MPI_Datatype mpi_fish_group;
MPI_Datatype mpi_net;
void create_mpi_datatypes();

int main(int argc, char** argv)
{
    srand(time(NULL));
    int numtasks, worldrank, rank, i, tag=1;
    int coords[2];
    int dims[2], periods[2]={0, 0}, reorder=0;

    int cart_coords[2];
    int target_rank;

    dims[0] = X;
    dims[1] = Y;

    // Two arrays of fish, one for the whole (only used in initialization
    // and the other to hold each process's groups)
    fish_group my_groups[POPULATION];
    int num_fish_in_cell = 0;

    net nets[NETS];

    MPI_Comm cartcomm;

    // createDimensions(dims, SIZE);

    MPI_Init(&argc, &argv);

    create_mpi_datatypes();

    // Get rank and size of the world
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldrank);

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cartcomm);
    MPI_Comm_rank(cartcomm, &rank);

    MPI_Cart_coords(cartcomm, rank, 2, coords);

    if(rank == 0){
        // Populate the world
        printf("World: %dx%d (size:%dx%d)\n", dims[0], dims[1], WORLD_HEIGHT, WORLD_WIDTH);
        populate(my_groups, POPULATION, WORLD_WIDTH, WORLD_HEIGHT);
        num_fish_in_cell = POPULATION;
        for(i = 0; i < POPULATION; i++){
            printf("fish: %d, x=%d, y=%d\n", my_groups[i].num, my_groups[i].x, my_groups[i].y);
        }
        for(i = 0; i < NETS; i++){
            nets[i] = get_net(WORLD_WIDTH, WORLD_HEIGHT);
            printf("net: %d, x=%d, y=%d\n", i, nets[i].x, nets[i].y);
        }
    }

    MPI_Bcast(nets, NETS, mpi_net, 0, cartcomm);

    int j = 0;

    fish_group received[POPULATION];
    MPI_Request recv_request;

    MPI_Irecv(received, POPULATION, mpi_fish_group,
        MPI_ANY_SOURCE, tag, cartcomm, &recv_request);
    while(j++ < ITERATIONS){
        int sends = 0;
        int testdone, probedone;

        MPI_Request requests[numtasks];
        // MPI_Status status[numtasks];

        MPI_Status recv_status, probe_status;


        int send_counts[numtasks];
        for(i = 0; i < numtasks; i++){
            send_counts[i] = 0;
        }

        fish_group send_objects[numtasks][num_fish_in_cell];

        for(i = 0; i < num_fish_in_cell; i++){
            if(my_groups[i].num == 0){
                remove_element(my_groups, i, num_fish_in_cell--);
                i--;
                continue;
            }
            get_cart_coords(cart_coords, &my_groups[i], WORLD_WIDTH, WORLD_HEIGHT, dims[0], dims[1]);
            MPI_Cart_rank(cartcomm, cart_coords, &target_rank);
            if(rank != target_rank){
                int sc = send_counts[target_rank];
                send_counts[target_rank] += 1;
                send_objects[target_rank][sc] = my_groups[i];
                remove_element(my_groups, i, num_fish_in_cell--);
                i--;
            }
        }

        for(i = 0; i < numtasks; i++){
            if(send_counts[i] > 0){
                printf("Rank %d: Sending %d groups to %d in loop %d\n", rank, send_counts[i], i, j);
                MPI_Isend(send_objects[i], send_counts[i], mpi_fish_group,
                    i, tag, cartcomm, &requests[sends++]);
                // MPI_Send(send_objects[i], send_counts[i], mpi_fish_group,
                //     i, tag, cartcomm);
            }
        }
        MPI_Barrier(cartcomm);
        struct timespec tim;
        tim.tv_sec  = 0;
        // tim.tv_nsec = 500000000L;
        tim.tv_nsec = 50000000L;
        nanosleep(&tim, NULL);

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
                printf("--fish-%d-%d-%d-%d-%d\n", j, my_groups[i].num, my_groups[i].x, my_groups[i].y, rank);
            }
            // MPI_Barrier(cartcomm);
            // if(rank==0){
            //     printf("--endstat\n");
            // }
            // MPI_Barrier(cartcomm);
        }

        int last_catch[NETS];
        if(rank == 0){
            for(i=0; i < NETS; i++){
                last_catch[i] = nets[i].fish;
            }
        }

        // Update the x,y position of every group according to it's movement speed.
        update(my_groups, num_fish_in_cell, nets, NETS);


        // printf("Gathering...\n");

        int recvnets = NETS*numtasks;
        net temp_nets[recvnets];

        MPI_Gather(nets, NETS, mpi_net, temp_nets, NETS, mpi_net, 0, cartcomm);

        // printf("Gather complete\n");
        // sleep(1);
        if(rank == 0){
            int new_catch[NETS];
            // printf("first\n");
            for(i=0; i < NETS; i++){
                new_catch[i] = 0;
                // printf("New catch %d: %d\n", i, new_catch[i]);
                // printf("fish in net: %d, last_catch: %d\n", nets[i].fish, last_catch[i]);
            }
            // printf("second\n");
            for(i=0; i < recvnets; i++){
                // printf("i=%d\n", i);
                int ind = i % NETS;
                // printf("ind: %d\n", ind);
                int diff = temp_nets[i].fish - last_catch[ind];
                // printf("temp_nets[i].fish=%d, last_catch[ind]=%d\n", temp_nets[i].fish, last_catch[ind]);
                // printf("diff: %d\n", diff);
                new_catch[ind] += diff;
                // printf("---------------\n");
            }
            // printf("third\n");
            for(i=0; i < NETS; i++){
                // printf("new_catch for %d: %d\n", i, new_catch[i]);
                nets[i].fish += new_catch[i];
            }
            printf("fourth\n");
            for(i=0; i < NETS; i++){
                printf("Catch for %d was %d\n", i, new_catch[i]);
                printf("Net %d has %d fish\n", i, nets[i].fish);
            }
        }

        MPI_Bcast(nets, NETS, mpi_net, 0, cartcomm);
    }

    MPI_Finalize();
    return 0;
}

void create_mpi_datatypes(){
    // Set up parameters to create our fish mpi datatype
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

    // Set up parameters to create our net mpi datatype
    int blocklengths2[3] = {1, 1, 1};
    MPI_Datatype types2[3] = {MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offsets2[3];

    offsets2[0] = offsetof(net, fish);
    offsets2[1] = offsetof(net, x);
    offsets2[2] = offsetof(net, y);

    // Create our mpi datatype
    MPI_Type_create_struct(3, blocklengths2, offsets2, types2, &mpi_net);
    MPI_Type_commit(&mpi_net);
}
