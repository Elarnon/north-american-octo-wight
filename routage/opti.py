import parse.py

def optimize(nstep, cars, rues, intersections, taken):
    for i in xrange(nstep):
        step(cars, rues, intersections, taken)
