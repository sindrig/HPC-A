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
    int i, n, x, y, sx, sy;
    srand(time(NULL));
    for(i=0; i < numfish; i++){
        grid[i].num = rand() % 100;
        grid[i].x = rand() % max_x;
        grid[i].y = rand() % max_y;
        grid[i].x_speed = rand() % 2;
        grid[i].y_speed = rand() % 2;
    }
}
