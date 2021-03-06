from parse import *
from random import *

GAIN_MULT = 5

def trivial(cars, inters, rues, time, nvehic, start, all_cars):
    cars = PriorityQueue(nvehic)
    for car in all_cars:
        cars.put(car)

    ## TODO : improve parameter
    diasporaTime = time / 3

## First part : diaspora
    while not cars.empty():
        c = cars.get()
        done = False
        best = None
        best_cost = None
        ##roads = inters[c.pos].goods()
        ##if len(roads) == 0:
        roads = inters[c.pos].alls
        for r in roads:
            if r.time + c.time < diasporaTime:
                cst = r.gainDiaspora(c)
                if best is None:
                    best = r
                    best_cost = cst
                elif cst > best_cost:
                    best = r
                    best_cost = cst
        if best is not None:
            if best_cost < 0:
                best = choice(roads)
            r = best
            c.move(r.path(c.pos), r.time)
            r.ok = True
            done = True
        cars.task_done()
        if done:
            cars.put(c)

    print(nvehic)
    for c in all_cars:
        print(len(c.path))
        for i in c.path:
            print(i)

cars, inters, rues, time, nvehic, start, all_cars = parse(sys.stdin)
trivial(cars, inters, rues, time, nvehic, start, all_cars)
