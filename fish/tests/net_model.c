#include "net_model.h"

#include <stdlib.h>

net get_net(int max_x, int max_y){
    net res;
    res.fish = 0;
    res.x = rand() % (max_x - NET_SIZE * 2) + NET_SIZE;
    res.y = rand() % (max_y - NET_SIZE * 2) + NET_SIZE;
    return res;
}


int add_fish_to_net(net *net_instance){
    if(net_instance->fish < MAX_FISH_IN_NET){
        net_instance->fish += 1;
        return 1;
    }
    return 0;
}
