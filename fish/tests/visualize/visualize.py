#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pickle
import sys
import random


def print_fish(data):
    import PIL
    import matplotlib.pyplot as plt
    from images2gif import writeGif
    fishes = sorted(
        [item for item in data if isinstance(item, Fish)],
        key=lambda x: x.iteration
    )
    settings = dict(
        (x.name, x.value) for x in data if isinstance(x, Parameter)
    )
    nets_lst = sorted(
        [item for item in data if isinstance(item, Net)],
        key=lambda x: (x.iteration, x._id)
    )
    nets = {}
    for nt in nets_lst:
        if not nt.iteration in nets:
            nets[nt.iteration] = []
        nets[nt.iteration].append(nt)
    num_procs = len(set(x.rank for x in fishes))

    # We assume that the grid is nxn
    num_x = num_y = num_procs**(0.5)

    colors = [[]] * num_procs
    avail_colors = [
        '#c90707', '#fcfaf2', '#a700b3', '#0dc4e0', '#ff9305', '#520b6b',
        '#fcce1c', '#ffdc08', '#f5f5dc', '#75ad3e', '#03c6ff', '#751ec3',
        '#9b8c75', '#f7f6ee', '#c9c9ff', '#b6fcd5', '#660066', '#ffdb00',
        '#d01265', '#dc322f', '#2aa198', '#000000', '#00dd2f', '#d9d7d7',
        '#2f2f2f', '#7fffd4', '#0edbec', '#00ff7f', '#00ced1', '#faebd7',
        '#ff4040'
    ]
    for i in range(len(colors)):
        colors[i] = random.choice(avail_colors)
        avail_colors.remove(colors[i])
    imgs = []

    def get_img(coll):
        # import io
        # points = [(fish.x, fish.y) for fish in coll]
        fig = plt.figure()
        x_size = settings['WORLD_WIDTH']
        y_size = settings['WORLD_HEIGHT']
        plt.xlim([0, x_size])
        plt.ylim([0, y_size])
        mainplt = fig.add_subplot(111)
        ax = fig.add_subplot(111)
        total_fish = 0
        for fish in sorted(coll, key=lambda x: -x.num):
            mainplt.plot(
                [fish.x],
                [fish.y],
                'o',
                color=colors[fish.rank],
                label='%d' % fish.num
            )
            ax.annotate(
                '%d' % fish.num,
                xy=(fish.x, fish.y),
                # textcoords='offset points'
            )
            total_fish += fish.num
        for net in nets.get(fish.iteration, []):
            circle = plt.Circle(
                (net.x, net.y), net.size, color='r', fill=False)
            fig.gca().add_artist(circle)
            ax.annotate(
                '%d' % net.fish,
                xy=(net.x, net.y)
            )

        plt.title('Iteration: %d - #Fish: %d' % (fish.iteration, total_fish))

        x_pos = x_size / num_x
        y_pos = y_size / num_y
        while x_pos < x_size:
            ax.plot([x_pos, x_pos], [0, y_size], 'k')
            x_pos += x_size / num_x
        while y_pos < x_size:
            ax.plot([0, x_size], [y_pos, y_pos], 'k')
            y_pos += y_size / num_y

        box = mainplt.get_position()
        mainplt.set_position([box.x0, box.y0, box.width * 0.8, box.height])
        mainplt.legend(loc='center left', bbox_to_anchor=(1, 0.5))

        # buf = io.BytesIO()
        canvas = plt.get_current_fig_manager().canvas
        canvas.draw()
        pil_image = PIL.Image.fromstring(
            'RGB', canvas.get_width_height(),
            canvas.tostring_rgb()
        )
        # pil_image.save(buf, 'PNG')
        plt.close()

        return pil_image

    collection = []
    last_iter = -1
    for fish in fishes:
        if fish.iteration > last_iter and collection:
            imgs.append(get_img(collection))
            collection = []
        collection.append(fish)
        last_iter = fish.iteration
    if collection:
        imgs.append(get_img(collection))

    writeGif('visualisation.gif', imgs, duration=0.2)


def get_file_scp(username, password):
    # Assumes that the datafile resides at
    # ~/HPC-A/fish/tests/output.dat
    import paramiko
    import scp
    client = paramiko.SSHClient()
    client.load_system_host_keys()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    client.connect('jotunn.rhi.hi.is', 22, username, password)
    scp_client = scp.SCPClient(client.get_transport())
    scp_client.get('~/HPC-A/fish/tests/output.dat')
    with open('output.dat', 'r') as f:
        return pickle.load(f)


class Fish(object):
    def __init__(self, iteration, num, x, y, rank):
        self.iteration = iteration
        self.num = num
        self.x = x
        self.y = y
        self.rank = rank

    def __unicode__(self):
        return '%d: (%d, %d): %d' % (self.iteration, self.x, self.y, self.num)

    __str__ = __unicode__


class Parameter(object):
    value = 0

    def __init__(self, name):
        self.name = name

    def get_defined_parameter(self):
        with open('first.c', 'r') as f:
            for line in f:
                if self.name in line and 'define' in line:
                    self.value = int(line.strip().split()[-1])

    def __unicode__(self):
        return '%s = %d' % (self.name, self.value)

    __str__ = __unicode__


class Net(object):
    def __init__(self, iteration, _id, fish, size, x, y):
        self.iteration = iteration
        self._id = _id
        self.fish = fish
        self.x = x
        self.y = y
        self.size = size

if sys.argv[-1] == 'save':
    data = []
    for p in ('POPULATION', 'WORLD_HEIGHT', 'WORLD_WIDTH'):
        param = Parameter(p)
        param.get_defined_parameter()
        data.append(param)
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
                try:
                    iteration, num, x, y, rank = (
                        int(k or '0') for k in parts[1:]
                    )
                except:
                    print 'Error with line!'
                    print '========================================='
                    print line
                    print '========================================='
                else:
                    data.append(Fish(iteration, num, x, y, rank))
            elif parttype == 'net':
                args = (int(k) for k in parts[1:])
                data.append(Net(*args))
        else:
            print line.strip()
    with open('output.dat', 'w') as f:
        pickle.dump(data, f)
elif len(sys.argv) == 3 and sys.argv[1] == 'gif':
    with open(sys.argv[-1], 'r') as f:
        print_fish(pickle.load(f))
elif len(sys.argv) == 4 and sys.argv[1] == 'gif':
    print_fish(get_file_scp(*sys.argv[2:]))
