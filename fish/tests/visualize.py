#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys

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
            num, x, y = (int(k) for k in parts[1:])
            print '%d: (%d, %d)' % (num, x, y)
