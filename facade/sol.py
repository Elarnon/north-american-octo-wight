from parse import *
import sys

def good(pos):
    return pos.val and not pos.cur

def bad(pos):
    return not pos.val and not pos.cur

def cost_diff(tmp):
    if good(tmp):
        return -1
    elif bad(tmp):
        return 1
    else:
        return 0

def try_expand(sq):
    if sq.add_cost() > 0:
        return False
    s = sq.s
    pic = sq.pic
    if sq.l - s > 1 and sq.c - s > 1:
        # Try expanding up and left
        diff = 0
        for i in xrange(sq.l - s - 2, sq.l - s):
            for j in xrange(sq.c - s, sq.c + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for j in xrange(sq.c - s - 2, sq.c - s):
            for i in xrange(sq.l - s, sq.l + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for i in xrange(sq.l - s - 2, sq.l - s):
            for j in xrange(sq.c - s - 2, sq.c - s):
                diff = diff + cost_diff(pic.get(i, j))
        if diff <= 0:
            sq.l = sq.l - 2
            sq.c = sq.c - 2
            sq.s = sq.s + 1
    if sq.l - s > 1 and sq.c + s < pic.ncols - 3:
        # Try expanding down and left
        diff = 0
        for i in xrange(sq.l - s - 2, sq.l - s):
            for j in xrange(sq.c - s, sq.c + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for j in xrange(sq.c + s + 1, sq.c + s + 3):
            for i in xrange(sq.l - s, sq.l + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for i in xrange(sq.l - s - 2, sq.l - s):
            for j in xrange(sq.c + s + 1, sq.c + s + 3):
                diff = diff + cost_diff(pic.get(i, j))
        if diff <= 0:
            sq.l = sq.l - 2
            sq.c = sq.c + 2
            sq.s = sq.s + 1
    if sq.l + s < pic.nlines - 3 and sq.c - s > 1:
        # Try expanding up and left
        diff = 0
        for i in xrange(sq.l + s + 1, sq.l + s + 3):
            for j in xrange(sq.c - s, sq.c + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for j in xrange(sq.c - s - 2, sq.c - s):
            for i in xrange(sq.l - s, sq.l + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for i in xrange(sq.l + s + 1, sq.l + s + 3):
            for j in xrange(sq.c - s - 2, sq.c - s):
                diff = diff + cost_diff(pic.get(i, j))
        if diff <= 0:
            sq.l = sq.l + 2
            sq.c = sq.c - 2
            sq.s = sq.s + 1
    if sq.l + s < pic.nlines - 3 and sq.c + s < pic.ncols - 3:
        # Try expanding up and left
        diff = 0
        for i in xrange(sq.l + s + 1, sq.l + s + 3):
            for j in xrange(sq.c - s, sq.c + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for j in xrange(sq.c + s + 1, sq.c + s + 3):
            for i in xrange(sq.l - s, sq.l + s + 1):
                diff = diff + cost_diff(pic.get(i, j))
        for i in xrange(sq.l + s + 1, sq.l + s + 3):
            for j in xrange(sq.c + s + 1, sq.c + s + 3):
                diff = diff + cost_diff(pic.get(i, j))
        if diff <= 0:
            sq.l = sq.l + 2
            sq.c = sq.c + 2
            sq.s = sq.s + 1
    return s != sq.s
    
def add_some(pic):
    for l in xrange(0, pic.nlines ):
        for c in xrange(0, pic.ncols):
            sq = square(pic, l, c, 0)
            while try_expand(sq):
                pass
            if sq.cost_add() < 0:
                sq.add()

def rm_useless(pic):
    rm = []
    for sq in pic.sqs:
        if sq.cost_rm() < 0:
            rm.append(sq)
    for sq in rm:
        if sq.cost_rm() < 0:
            sq.rm()

def doit():
    pic = picture()
    pic.parse(sys.stdin)
    add_some(pic)
    rm_useless(pic)
    pic.display()

doit()
