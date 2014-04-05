from math import *

def diaspora(londre, center, costs, inters):
    r2 = sqrt(2)
    kappa = 3   # power parameter for gain (odd number)
    dirs = [(1,0), (r2, r2), (0,1), (-r2, r2), (-1,0), (-r2, -r2), (0, -1), (r2, -r2)]
    max_buts = [(londre,0) for x in xrange(0,8)]
    for but in inters:
        for i in xrange(0,8):
            (x,y) = dirs[i]
            gain0 = (x * (but.lat - center.lat) + y * (but.lon - center.lon))
            gain = copysign(gain0 **  kappa, gain0) / cost[londre.key][but.key]
            (prev_pos, prev_gain) = max_buts[i]
            if prev_gain < gain :
                max_buts[i] = (but, gain)
    return [ x for (x,y) in max_buts ]



def main_diaspora(londre, center, inters, rues):
    (costs, paths) = to_matrix(inters, rues)
    (costs, paths) = floy_warshall(costs, paths)
    final_but = diaspora(londre, center, costs, inters)
    [ paths[londre.id][but.id] for but in final_but ]
