class square(object):
    def __init__(self, pic, l, c, s):
        self.pic = pic
        self.l = l
        self.c = c
        self.s = s

    def iter(self):
        l, c, s = self.l, self.c, self.s
        for i in xrange(l - s, l + s + 1):
            for j in xrange(c - s, c + s + 1):
                yield (i, j)

    def cost_add(self):
        c = 1
        for (i, j) in self.iter():
            tmp = self.pic.get(i, j)
            if tmp.val == False and tmp.cur == False:
                # We will need an erase
                c = c + 1
            elif tmp.val == True and tmp.cur == False:
                # We improve !
                c = c - 1
        return c

    def cost_rm(self):
        c = -1
        for (i, j) in self.iter():
            tmp = self.pic.get(i, j)
            if tmp.val == False:
                # Good !
                c = c - 1
            elif tmp.val == True and len(tmp.sqs) == 1:
                # Bad !
                c = c + 1
        return c

    def rm(self):
        pic = self.pic
        pic.fix_cost += self.cost_rm()
        for (i, j) in self.iter():
            tmp = pic.get(i, j)
            tmp.sqs.remove(self)
            if len(tmp.sqs) == 0:
                tmp.cur = False
        pic.sqs.remove(self)

    def add(self):
        pic = self.pic
        pic.fix_cost += self.cost_add()
        pic.sqs.add(self)
        for (i, j) in self.iter():
            tmp = pic.get(i, j)
            tmp.cur = True
            tmp.sqs.add(self)

class point(object):
    def __init__(self, val):
        self.val = val
        self.cur = False
        self.sqs = set([])

class picture(object):
    def parse(self, f):
        l = f.readline().split(' ')
        nlines = int(l[0])
        ncol = int(l[1])
        arr = []
        for line in xrange(0, nlines):
            l = f.readline()
            for col in xrange(0, ncol):
                arr.append(point((l[col] == '#')))
        self.nlines = nlines
        self.ncols = ncol
        self.arr = arr
        self.sqs = set([])
        self.fix_cost = nlines * ncol
        self.fix = []
        
    def get(self, l, c):
        return self.arr[l + c * self.nlines]

    def score(self):
        return len(self.sqs) + len(self.fix)

    def fixit(self):
        self.fix = []
        for line in xrange(0, self.nlines):
            for col in xrange(0, self.ncols):
                tmp = self.get(line, col)
                if tmp.val == True and tmp.cur == False:
                    self.fix.append('PAINTSQ {} {} 0'.format(line, col))
                elif tmp.val == False and tmp.cur == True:
                    self.fix.append('ERASECELL {} {}'.format(line, col))

    def display(self):
        self.fixit()
        print(self.score())
        for sq in self.sqs:
            print('PAINTSQ {} {} {}'.format(sq.l, sq.c, sq.s))
        for i in self.fix:
            print(i)