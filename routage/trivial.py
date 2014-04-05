from parse import *

GAIN_MULT = 100

def trivial(cars, inters, rues, time, nvehic, all_cars):
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
    for c in all_cars:
        print(len(c.path))
        for i in c.path:
            print(i)

cars, inters, rues, time, nvehic, all_cars = parse(sys.stdin)
trivial(cars, inters, rues, time, nvehic, all_cars)