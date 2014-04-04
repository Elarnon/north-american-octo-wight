import sys

def trivial(f):
    l = f.readline().split(' ')
    nlines = int(l[0])
    ncol = int(l[1])
    res = []
    for line in xrange(0, nlines):
        l = f.readline()
        for col in xrange(0, ncol):
            if l[col] == '#':
                res.append('PAINTSQ {} {} 0'.format(line, col))
    print(len(res))
    for i in res:
        print(i)
        
trivial(sys.stdin)
