#ifndef __NET_MODEL__
#define __NET_MODEL__

#define MAX_FISH_IN_NET 200
#define NETS 2
#define NET_SIZE 40

struct net_s {
    // Number of fish in the net
    int fish;
    // Position on the x-axis
    int x;
    // Position on the y-axis
    int y;
};

typedef struct net_s net;

net get_net(int max_x, int max_y);
int add_fish_to_net(net *net_instance);

// void populate(fish_group *grid, int numfish, int max_x, int max_y);
// void get_cart_coords(int *info, fish_group *group, int max_x, int max_y, int num_x, int num_y);
// void update(fish_group *groups, int numfish);
#endif
