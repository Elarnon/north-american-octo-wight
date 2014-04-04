import sys
from parse import picture

o = picture()
o.parse(sys.stdin)
for i in xrange(0, o.nlines):
    for j in xrange(0, o.ncols):
        if o.get(i, j)['val']:
            o.printsq(i, j)
o.display()