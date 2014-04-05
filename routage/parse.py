from decimal import *
from Queue import *
import sys
import random

GAIN_MULT = 2
PROFONDEUR = 10

class voiture(object):
    def __init__(self, inters, rues, pos):
        self.inters = inters
        self.rues = rues
        self.pos = pos
        self.time = 0
        self.path = [pos]

    def __cmp__(self, other):
        if self.time < other.time:
            return -1
        elif self.time == other.time:
            return 0
        else:
            return 1

    def move(self, npos, cost):
        self.pos = npos
        self.path.append(npos)
        self.time = self.time + cost

class intersection(object):
    def __init__(self, inters, rues, id, lat, lon):
        self.inters = inters
        self.rues = rues
        self.lat = lat
        self.lon = lon
        self.alls = []
        self.id = id

    def goods(self):
        return [r for r in self.alls if r.ok]

class rue(object):
    def __init__(self, inters, rues, start, end, bidir, cost, score):
        self.inters = inters
        self.rues = rues
        self.ok = False
        inters[start].alls.append(self)
        self.bidir = bidir
        self.start = start
        self.end = end
        if bidir:
            inters[end].alls.append(self)
        self.time = cost
        self.score = score

    def path(self, start):
        if start == self.start:
            return self.end
        elif start == self.end and self.bidir:
            return self.start
        else:
            return None

    def raw(self):
        return (float(self.score * 1000000) / float(self.time))

    def real_gain(self, pos, k):
        if k == 0:
            return self.raw()
        nxt = self.path(pos)
        c = self.raw()
        for r in self.inters[nxt].goods():
            c += r.real_gain(nxt, k-1)
        return c

    # Plus le cout est faible plus on veut aller sur la rue
    def gain(self, pos):
        rc = self.real_gain(pos, PROFONDEUR)
        return rc

def trivial(f):
    l = f.readline().split(' ')
    ninter = int(l[0])
    nrues = int(l[1])
    time = int(l[2])
    nvehic = int(l[3])
    start = int(l[4])
    inters = [None] * ninter
    rues = [None] * nrues
    go = []
    for inter in xrange(0, ninter):
        l = f.readline().split(' ')
        lat = Decimal(l[0])
        lon = Decimal(l[1])
        inters[inter] = intersection(inters, rues, inter, lat, lon)
    for ru in xrange(0, nrues):
        l = f.readline().split(' ')
        a = int(l[0])
        b = int(l[1])
        d = int(l[2]) == 2
        cost = int(l[3])
        score = int(l[4])
        r = rue(inters, rue, a, b, d, cost, score)
        rues[ru] = r
    cars = PriorityQueue(nvehic)
    stuff = set([])
    for i in xrange(0, nvehic):
        c = voiture(inters, rues, start)
        cars.put(c)
        stuff.add(c)

    while not cars.empty():
        c = cars.get()
        done = False
        for r in inters[c.pos].alls:
            best = None
            best_cost = None
            if not r.ok and r.time + c.time < time:
                cst = r.gain(c.pos)
                if best is None:
                    best = r
                    best_cost = cst
                elif cst > best_cost:
                    best = r
                    best_cost = cst
        if best is not None:
            r = best
            c.move(r.path(c.pos), r.time)
            r.ok = True
            done = True
        if not done:
            oks = []
            best_gain = 0.0
            for r in inters[c.pos].alls:
                if r.time + c.time < time:
                    cst = r.gain(c.pos)
                    if cst * GAIN_MULT > best_gain:
                        oks.append(r)
                        if cst > best_gain:
                            best_gain = cst
            if len(oks) > 0:
                r = random.choice(oks)
                c.move(r.path(c.pos), r.time)
                done = True
        cars.task_done()
        if done:
            cars.put(c)

    print(nvehic)
    for c in stuff:
        print(len(c.path))
        for i in c.path:
            print(i)

trivial(sys.stdin)
