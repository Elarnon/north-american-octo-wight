import sys
from parse import picture, square, point

o = picture()
o.parse(sys.stdin)
for i in xrange(0, o.nlines):
    for j in xrange(0, o.ncols):
        if o.get(i, j).val:
            square(o, i, j, 0).add()
o.display()
