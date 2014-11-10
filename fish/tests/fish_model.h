#ifndef __FISH_MODEL__
#define __FISH_MODEL__

#define MAX_SPEED 8
#define MAX_SPEED_CHANGE 3

struct fish_group_s {
    // Number of fish in the group
    int num;
    // Position on the x-axis
    int x;
    // Position on the y-axis
    int y;
    // Speed in the x-direction
    int x_speed;
    // Speed in the y-direction
    int y_speed;
};

typedef struct fish_group_s fish_group;

void populate(fish_group *grid, int numfish, int max_x, int max_y);
void get_cart_coords(int *info, fish_group *group, int max_x, int max_y, int num_x, int num_y);
int get_new_speed(int old_speed);
#endif
