from math import *
from floyd_warshall import *


def diaspora(londre, costs, inters):
    centerX = 48.859800
    centerY = 2.340953
    r2 = sqrt(2)
    kappa = 3   # power parameter for gain (odd number)
    dirs = [(1,0), (r2, r2), (0,1), (-r2, r2), (-1,0), (-r2, -r2), (0, -1), (r2, -r2)]
    max_buts = [(londre,0) for x in xrange(0,8)]
    for but in inters:
        for i in xrange(0,8):
            (x,y) = dirs[i]
            gain0 = (x * (but.lat - centerX) + y * (but.lon - centerY))
            gain = copysign(gain0 **  kappa, gain0) / cost[londre.key][but.key]
            (prev_pos, prev_gain) = max_buts[i]
            if prev_gain < gain :
                max_buts[i] = (but, gain)
    return [ x for (x,y) in max_buts ]

def main_diaspora(londres, center, inters, rues):
    (costs, paths) = to_matrix(inters, rues)
    (costs, paths) = floy_warshall(costs, paths)
    final_but = diaspora(londres, center, costs, inters)
    return [ paths[londres.id][but.id] for but in final_but ]

def main_diaspora_LondonCenter(londres, inters, rues):
    return main_diaspora(londres, londres, inters, rues)

