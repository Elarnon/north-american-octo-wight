def to_matrix(inters, rues):
    costs = []
    paths = []

    # initialisation
    for n in inters:
        tmp_costs = []
        tmp_paths = []
        for m in inters:
            tmp_paths.append([])
            if n == m:
                tmp_costs.append(0)
            else:
                tmp_costs.append(+float("inf"))
        costs.append(tmp_costs)
        paths.append(tmp_paths)

    # filling
    for r in rues:
        costs[r.start][r.end] = r.cost
        paths[r.start][r.end] = [r]
        if r.bidir:
            costs[r.end][r.start] = r.cost
            paths[r.start][r.end] = [r]

    return (costs, paths)

def floyd_warshall(costs, paths):
    n = len(costs)
    for k in xrange(0,n):
        for i in xrange(0,n):
            for j in xrange(0,n):
                c = costs[i][k] + costs[k][j]
                if costs[i][j] > c:
                    costs[i][j] = c
                    paths[i][j] = paths[i][k] + paths[k][j]
    return (costs, paths)

