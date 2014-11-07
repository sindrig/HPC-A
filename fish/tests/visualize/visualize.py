#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pickle
import sys

with open('first.c', 'r') as f:
    for line in f:
        if 'POPULATION' in line and 'define' in line:
            nfish = int(line.strip().split()[-1])
            break

fish_received = []


def print_fish(fishes):
    pass


class Fish(object):
    def __init__(self, iteration, num, x, y):
        self.iteration = iteration
        self.num = num
        self.x = x
        self.y = y

if sys.argv[-1] == 'save':
    while 1:
        try:
            line = sys.stdin.readline()
        except KeyboardInterrupt:
            pass
        if not line:
            break
        if line.startswith('--'):
            line = line.lstrip('--').strip()
            parts = line.split('-')
            parttype = parts[0]
            if parttype == 'fish':
                iteration, num, x, y = (int(k) for k in parts[1:])
                fish_received.append(Fish(iteration, num, x, y))
    with open('output.dat', 'w') as f:
        pickle.dump(fish_received, f)
elif len(sys.argv) > 2 and sys.argv[1] == 'gif':
    with open(sys.argv[-1], 'r') as f:
        print_fish(pickle.load(f))
