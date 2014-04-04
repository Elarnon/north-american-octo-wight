import parse

def create_table(pict):
    p = (min(pict.nlines, pict.ncols) - 1) / 2 
    t = []
    for k in range(0, p):
        for c in range(k+1, pict.ncols-k+1):
            for l in range(k+1, pict.nlines-k+1):
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



