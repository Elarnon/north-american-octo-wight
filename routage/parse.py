from decimal import *
from Queue import *
import sys

class voiture(object):
    def __init__(self, inters, rues, pos):
        self.inters = inters
        self.rues = rues
        self.pos = pos
        self.time = 0
        self.path = [pos]

    def __cmp__(self, other):
        if self.pos < other.pos:
            return -1
        elif self.pos == other.pos:
            return 0
        else:
            return 1

class intersection(object):
    def __init__(self, inters, rues, lat, lon):
        self.inters = inters
        self.rues = rues
        self.lat = lat
        self.lon = lon
        self.alls = []

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
        self.cost = cost
        self.score = cost
        self.cars = set([])

    def path(self, start):
        if start == self.start:
            return self.end
        elif start == self.end and self.bidir:
            return self.start
        else:
            return None

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
        inters[inter] = intersection(inters, rues, lat, lon)
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
        # TODO: Not updated
        stuff.add(c)

    time = 100
        
    while not cars.empty():
        c = cars.get()
        done = False
        for r in inters[c.pos].alls:
            if not r.ok and r.cost + c.time < time:
                c.pos = r.path(c.pos)
                c.time = c.time + r.cost
                r.ok = True
                done = True
                break
        if not done:
            for r in inters[c.pos].alls:
                if r.cost + c.time < time:
                    c.pos = r.path(c.pos)
                    c.path.append(c.pos)
                    c.time = c.time + r.cost
                    r.ok = True
                    done = True
                    break
        cars.task_done()
        if done:
            cars.put(c)

    print(nvehic)
    for c in stuff:
        print(len(c.path))
        for i in c.path:
            print(i)

trivial(sys.stdin)
