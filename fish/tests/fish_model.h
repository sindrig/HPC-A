#ifndef __FISH_MODEL__
#define __FISH_MODEL__

// Create dimensions for our world
void createDimensions(int* arr, int size);

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

#endif
