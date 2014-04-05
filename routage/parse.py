from decimal import *
from Queue import *
import sys
import random

PROFONDEUR = 100

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
        if self.ok:
            real_score = 0
        else:
            real_score = self.score
        return (real_score, self.time)

    def real_gain(self, pos, k):
        if k == 0:
            return self.raw()
        nxt = self.path(pos)
        best_score = 0
        best_time = 1
        for r in self.inters[nxt].goods():
            (nscore, ntime) = r.real_gain(nxt, k-1)
            if float(nscore) / float(ntime) > float(best_score) / float(best_time):
                best_score = nscore
                best_time = ntime
        myscore, mytime = self.raw()
        return (best_score + myscore, best_time + mytime)

    # Plus le cout est faible plus on veut aller sur la rue
    def gain(self, pos):
        (score, time) = self.real_gain(pos, PROFONDEUR)
        return float(score) / float(time)

def parse(f):
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
    all_cars = set([])
    for i in xrange(0, nvehic):
        c = voiture(inters, rues, start)
        cars.put(c)
        all_cars.add(c)

    return (cars, inters, rues, time, nvehic, all_cars)