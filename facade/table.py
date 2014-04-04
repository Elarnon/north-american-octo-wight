import parse

def create_table(pict):
    p = (min(pict.nlines, pict.ncols) - 1) / 2 
    t = []
    for i in xrange(0, p):
        tmp0 = []
        for c in xrange(0, pict.ncols):
            tmp1 = []
            for l in xrange(0, pict.nlines):
                tmp0.append(tmp1)
        t.append(tmp0)
    for k in xrange(0, p):
        for c in xrange(k+1, pict.ncols-k+1):
            for l in xrange(k+1, pict.nlines-k+1):
                t[c][l][k] = 0
                if k == 0:
                    if pict.get(l,c):
                        t[c][l][k] = 1
                    else:
                        t[c][l][k] = 0
                else :
                    for x in [-k,k]:
                        for y in range(-k,k):
                            t[c][l][k] = t[c][l][k] + t[c+x][l+y][0] + t[c+y][l+x][0]
    return t