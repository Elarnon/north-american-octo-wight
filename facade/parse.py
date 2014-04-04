class picture(object):
    def parse(self, f):
        l = f.readline().split(' ')
        nlines = int(l[0])
        ncol = int(l[1])
        arr = []
        for line in xrange(0, nlines):
            l = f.readline()
            for col in xrange(0, ncol):
                arr.append({ "val": (l[col] == '#'), "cur": False, "sqs": [] })
        self.nlines = nlines
        self.ncols = ncol
        self.arr = arr
        self.prints = []
        self.erase = []
        
    def get(self, l, c):
        return self.arr[l + c * self.nlines]

    def printsq(self, l, c, s=0):
        self.prints.append((l, c, s))
        for i in xrange(l - s, l + s + 1):
            for j in xrange(c - s, c + s + 1):
                tmp = self.get(i, j)
                tmp['cur'] = True
                tmp['sqs'].append((l, c, s))

    def erasecell(self, l, c):
        tmp = self.get(l, c)
        tmp['cur'] = False
        self.erases.append((l, c))

    def display(self):
        print(len(self.prints) + len(self.erase))
        for (l, c, s) in self.prints:
            print('PAINTSQ {} {} {}'.format(l, c, s))
        for (l, c) in self.erase:
            print('ERASECELL {} {}'.format(l, c))