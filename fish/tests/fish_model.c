#include "fish_model.h"
#include "net_model.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>


void createDimensions(int* arr, int size){
    int x, y;
    x = 1;
    while(1){
        if(x == size){
            y = 1;
            break;
        }
        y = size / x;
        if(x >= y){
            if(x * y == size){
                break;
            }
        }
        x++;
    }
    arr[0] = x;
    arr[1] = y;
}

void populate(fish_group *grid, int numfish, int max_x, int max_y){
    int i;
    for(i=0; i < numfish; i++){
        grid[i].num = rand() % 100;
        grid[i].x = rand() % max_x;
        grid[i].y = rand() % max_y;
        grid[i].x_speed = (rand() % MAX_SPEED) - MAX_SPEED / 2;
        grid[i].y_speed = (rand() % MAX_SPEED) - MAX_SPEED / 2;
    }
}

int get_new_speed(int old_speed){
    // TODO: Make this somehow more realistic
    int new_speed = old_speed + (rand() % (MAX_SPEED_CHANGE*2+1)) - MAX_SPEED_CHANGE;
    if(new_speed > MAX_SPEED){
        return MAX_SPEED - 1;
    }
    if(new_speed < -MAX_SPEED){
        return -MAX_SPEED + 1;
    }
    return new_speed;
}


// max_x and max_y are the world size parameters
// num_x and num_y are the number of domains in our domain decomposition
void get_cart_coords(int *info, fish_group *group,
                     int max_x, int max_y,
                     int num_x, int num_y){
    int x = 0, y = 1;
    int org_x, org_y;
    org_x = group->x;
    org_y = group->y;
    if(group->x >= max_x){
        group->x = group->x - max_x;
    }else if(group->x < 0){
        group->x = max_x + group->x;
    }
    if(group->y >= max_y){
        group->y = group->y - max_y;
    }else if(group->y < 0){
        group->y = max_y + group->y;
    }

    info[x] = group->x / (max_x / num_x);
    info[y] = group->y / (max_y / num_y);
    // printf("Returning coords (%d, %d) from (%d, %d) (originally (%d, %d) (speed: (%d, %d)))\n",
    //     info[x], info[y], group->x, group->y, org_x, org_y, group->x_speed, group->y_speed);
}
