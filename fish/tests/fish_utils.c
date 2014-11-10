#include "fish_model.h"
#include "net_model.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void remove_element(fish_group *array, int index, int array_length)
{
   int i;
   for(i = index; i < array_length - 1; i++) array[i] = array[i + 1];
}


int is_inside(fish_group group, net net_instance){
    int x_dist, y_dist;
    x_dist = group.x-net_instance.x;
    y_dist = group.y-net_instance.y;
    return sqrt(x_dist*x_dist + y_dist*y_dist) < NET_SIZE;
}


void update(fish_group *groups, int numfish, net *nets, int numnets){
    // TODO: Change x_speed and y_speed
    int i, j;
    for(i=0; i < numfish; i++){
        for(j=0; j < NETS; j++){
            if(is_inside(groups[i], nets[j])){
                printf("Group is inside (%d,%d) E (%d,%d)\n", groups[i].x, groups[i].y, nets[j].x, nets[j].y);
                if(add_fish_to_net(&nets[j])){
                    groups[i].num -= 1;
                }
            }
        }
        groups[i].x += groups[i].x_speed;
        groups[i].y += groups[i].y_speed;
        groups[i].x_speed = get_new_speed(groups[i].x_speed);
        groups[i].y_speed = get_new_speed(groups[i].y_speed);
    }
}
