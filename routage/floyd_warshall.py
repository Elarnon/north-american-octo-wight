def to_matrix(inters, rues):
    costs = []
    for n in inters:
        tmp = []
        for m in inters:
            if n == m:
                tmp.append(0)
            else:
                tmp.append(+inf)
        costs.append(tmp)

    for r in rues:
        costs[r.start][r.end] = r.cost
        if r.bidir:
            costs[r.end][r.start] = r.cost
    return costs

def floyd_warshall(costs):
    n = len(costs)
    for k in xrange(0,n):
        for i in xrange(0,n):
            for j in xrange(0,n):
                costs[i][j] = min(costs[i][j], costs[i][k] + costs[k][j])
    return costs

