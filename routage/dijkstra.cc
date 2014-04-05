#include "main.h"
#include<boost/heap/fibonacci_heap.hpp>

using namespace boost;
using namespace std;

typedef std::pair<long, long> Edge;

Edge* edge_array;
long nb_edges;
long *weights;
long *dist;
long *previous;
list<long> *edges;
struct CmpDist {
  bool operator()(long i, long j) const {
    if (dist[i] == -1) {
      if (dist[j] == -1) {
        return i < j;
      } else {
        return false;
      }
    } else {
      return dist[i] < dist[j];
    }
  }
};

boost::heap::fibonacci_heap<long, boost::heap::compare<CmpDist>>::handle_type *handles;

void djikstra_init() {
  long nb = 0;
  dist = new long[ninters];
  previous = new long[ninters];
  edges = new list<long>[ninters];
  handles = new boost::heap::fibonacci_heap<long, boost::heap::compare<CmpDist>>::handle_type[ninters];;
  for (int i(0); i < ninters; ++i) {
    dist[i] = -1;
    previous[i] = -1;
    edges[i] = list<long>();
  }
  for (int i(0); i < nrues; ++i) {
    if (rues[i].bidir) {
      nb += 1;
    }
    nb += 1;
  }
  edge_array = new Edge[nb];
  weights = new long[nb];
  nb = 0;
  for (int i(0); i < nrues; ++i) {
    auto st = rues[i].start;
    auto en = rues[i].end;
    edges[st].push_back(nb);
    edge_array[nb] = make_pair(st, en);
    weights[nb] = rues[i].time;
    nb++;
    if (rues[i].bidir) {
      edges[en].push_back(nb);
      edge_array[nb] = make_pair(rues[i].end, rues[i].start);
      weights[nb] = rues[i].time;
      nb++;
    }
  }
  nb_edges = nb;
}

void dijkstra(long from) {
  dist[from] = 0;
  auto pq = boost::heap::fibonacci_heap<long, boost::heap::compare<CmpDist>>();
  for (int i(0); i < ninters; ++i) {
    handles[i] = pq.push(i);
  }
  while (!pq.empty()) {
    auto i = pq.top();
    pq.pop();
    if (dist[i] == -1) {
      break;
    }
    for (auto ed : edges[i]) {
      auto tmp = dist[i] + weights[ed];
      auto other = edge_array[ed].second;
      if (tmp < dist[other]) {
        dist[other] = tmp;
        previous[other] = i;
        pq.update(handles[other]);
      }
    }
  }
}

list<long> find_path(long to) {
  list<long> res = { to };
  while (previous[to] != -1) {
    res.push_front(to);
    to = previous[to];
  }
  return res;
}

list<long> find_one_path(std::vector<long> tos) {
  long min(-1);
  long mini;
  for (auto to : tos) {
    if (min == -1 || (dist[to] != -1 && dist[to] < min)) {
      min = dist[to];
      mini = to;
    }
  }
  return find_path(mini);
}
