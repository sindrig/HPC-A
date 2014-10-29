#include "fish_model.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>


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
    int max_speed = max_x / 10;
    int i, n, x, y, sx, sy;
    srand(time(NULL));
    for(i=0; i < numfish; i++){
        grid[i].num = rand() % 100;
        grid[i].x = rand() % max_x;
        grid[i].y = rand() % max_y;
        grid[i].x_speed = (rand() % max_speed) - max_speed / 2;
        grid[i].y_speed = (rand() % max_speed) - max_speed / 2;
    }
}

void update(fish_group *groups, int numfish){
    // TODO: Change x_speed and y_speed
    int i;
    for(i=0; i < numfish; i++){
        groups[i].x += groups[i].x_speed;
        groups[i].y += groups[i].y_speed;
    }
}


// max_x and max_y are the world size parameters
// num_x and num_y are the number of domains in our domain decomposition
void get_cart_coords(int *info, fish_group group,
                     int max_x, int max_y,
                     int num_x, int num_y){
    int x = 0, y = 1;
    if(group.x > max_x){
        group.x = group.x - max_x;
    }
    if(group.y > max_y){
        group.y = group.y - max_y;
    }

    info[x] = group.x / (max_x / num_x);
    info[y] = group.y / (max_y / num_y);
}
