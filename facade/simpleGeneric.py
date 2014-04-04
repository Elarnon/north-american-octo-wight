import sys

def parse(f):
    l = f.readline().split(' ')
    nlines = int(l[0])
    ncol = int(l[1])
    arr = []
    for line in xrange(0, nlines):
        l = f.readline()
        tmp = []
        for col in xrange(0, ncol):
            tmp.append(l[col] == '#')
        arr.append(tmp)
    return (nlines, ncol, arr)

nlines, ncol, arr = parse(sys.stdin)

# Detect simple squares
k = 2 
squareSize = 2*k + 1

res = []
for line in xrange(k, nlines - k):
    for col in xrange(k, ncol - k):
        tries = [(i, j) for i in xrange(line-k, line+k+1) for j in xrange(col-k, col+k+1)]
        hasone = False
        err = []
        for (i, j) in tries:
            if arr[i][j] is True:
                hasone = True
            if arr[i][j] is False:
                err.append((i, j))
                break
        if ok is True:
            res.append('PAINTSQ {} {} 1'.format(line, col))
            for (i, j) in tries:
                arr[i][j] = None

for line in xrange(0, nlines):
    for col in xrange(0, ncol):
        if arr[line][col] is True:
            res.append('PAINTSQ {} {} 0'.format(line, col))

print(len(res))
for i in res:
    print(i)
