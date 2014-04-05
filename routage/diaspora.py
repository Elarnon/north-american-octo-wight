from math import *
from floyd_warshall import *

def diaspora(src, costs, inters):
    r2 = sqrt(2)
    kappa = 3   # power parameter for gain (odd number)
    dirs = [(1,0), (r2, r2), (0,1), (-r2, r2), (-1,0), (-r2, -r2), (0, -1), (r2, -r2)]
    max_buts = [(src,0) for x in xrange(0,8)]
    for but in inters:
        for i in xrange(0,8):
            (x,y) = dirs[i]
            gain0 = (x * (but.lat - src.lat) + y * (but.lon - src.lon))
            gain = copysign(gain0 **  kappa, gain0) / cost[src.key][but.key]
            (prev_pos, prev_gain) = max_buts[i]
            if prev_gain < gain :
                max_buts[i] = (but, gain)
    return [ x for (x,y) in max_buts ]



def main_diaspora(src, inters, rues):
    (costs, paths) = to_matrix(inters, rues)
    (costs, paths) = floyd_warshall(costs, paths)
    final_but = diaspora(src, costs, inters)
    [ paths[src.id][but.id] for but in final_but ]
