import parse
import table

def compute_sqc(table, pict):
    p = (min(pict.nlines, pict.ncols) - 1) / 2 
    for k in xrange(p,-1,-1):
        for c in range(k+1, pict.ncols-k+1):
            for l in range(k+1, pict.nlines-k+1):
                if table[c][l][k] == (2*k+1)**2:
                    s = square(pict, l, c, k)
                    if s.cost_add() < 0:
                        s.add()
                    


                    
o = picture()
o.parse(sys.stdin)
t = create_table(o)
compute_sqc(t,o)
o.display()
