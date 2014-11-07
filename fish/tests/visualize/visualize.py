#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pickle
import sys


def print_fish(data):
    for item in data:
        print item


def get_file_scp(username, password):
    # Assumes that the datafile resides at
    # ~/HPC-A/fish/tests/visualize/output.dat
    import paramiko
    import scp
    client = paramiko.SSHClient()
    client.load_system_host_keys()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    client.connect('jotunn.rhi.hi.is', 22, username, password)
    scp_client = scp.SCPClient(client.get_transport())
    scp_client.get('~/HPC-A/fish/tests/visualize/output.dat')
    with open('output.dat', 'r') as f:
        return pickle.load(f)


class Fish(object):
    def __init__(self, iteration, num, x, y):
        self.iteration = iteration
        self.num = num
        self.x = x
        self.y = y

    def __unicode__(self):
        return '(%d, %d): %d' % (self.x, self.y, self.num)


class Parameter(object):
    value = 0

    def __init__(self, name):
        self.name = name

    def get_defined_parameter(self, f):
        for line in f:
            if self.name in line and 'define' in line:
                self.value = int(line.strip().split()[-1])

    def __unicode__(self):
        return '%s = %d' % (self.name, self.value)


if sys.argv[-1] == 'save':
    data = []
    with open('../first.c', 'r') as f:
        for p in ('POPULATION', 'WORLD_HEIGHT', 'WORLD_WIDTH'):
            param = Parameter(p)
            data.append(p.get_defined_parameter(f))
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
                data.append(Fish(iteration, num, x, y))
    with open('output.dat', 'w') as f:
        pickle.dump(data, f)
elif len(sys.argv) == 2 and sys.argv[1] == 'gif':
    with open(sys.argv[-1], 'r') as f:
        print_fish(pickle.load(f))
elif len(sys.argv) == 4 and sys.argv[1] == 'gif':
    print_fish(get_file_scp(*sys.argv[2:]))
