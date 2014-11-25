#ifndef __FISH_UTILS__
#define __FISH_UTILS__

void remove_element(fish_group *array, int index, int array_length);
void update(fish_group *groups, int numfish, net *nets, int numnets);
int is_inside(fish_group group, net net_instance);
#endif
