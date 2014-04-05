from decimal import *
import sys

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
        inters[inter] = { 'direct': [], 'rev': [] }
    for rue in xrange(0, nrues):
        l = f.readline().split(' ')
        a = int(l[0])
        b = int(l[1])
        d = int(l[2]) == 2
        inters[a]['direct'].append(rue)
        if d:
            inters[b]['rev'].append(rue)
        cost = int(l[3])
        lon = int(l[4])
        rues[rue] = (a, b, d, cost, lon)

    res = []
    nb = 0
    stuff = inters[start]['direct']
    stuff = stuff + stuff + stuff
    while nb < nvehic:
        (a, b, d, cost, lon) = rues[stuff[nb]]
        if cost <= time:
            nb = nb + 1
            l = [a, b]
            pos = b
            c = cost
            for rue in inters[b]['direct']:
                (a, b, d, cost, lon) = rues[rue]
                if c + cost <= time:
                    l.append(b)
                    break
            res.append(l)

    print(nvehic)

    for (l) in res:
        print(len(l))
        for j in l:
            print(j)

trivial(sys.stdin)