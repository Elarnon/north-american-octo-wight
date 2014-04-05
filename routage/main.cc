#include "main.h"
#include <queue>
#include <cstdio>

 Intersection *inters;
 long ninters;
 Rue *rues;
 long nrues;
 Car *cars;
 long nvehic;
 long start;
 long total_time;

struct CmpCar {
  bool operator()(int i, int j) {
    return cars[i].time < cars[j].time;
  }
};

int main() {
  parse();
  djikstra_init();
  dijkstra(start);
  auto pq = std::priority_queue<long, std::vector<long>, CmpCar>();
  for (int i(0); i < nvehic; ++i) {
    pq.push(i);
  }
  while (!pq.empty()) {
    auto i = pq.top();
    pq.pop();
    Car &c = cars[i];
    Rue* best(NULL);
    double best_cost = 0;
    for (Rue *r : inters[c.pos].alls) {
      if (r->time + c.time < total_time) {
        auto cst = r->gain(c.pos);
        if (cst > -1.0) {
          if (best == NULL || cst > best_cost) {
            best_cost = cst;
            best = r;
          }
        }
      }
    }
    if (best != NULL) {
      c.move(best->path(c.pos), best->time);
      best->ok = true;
      pq.push(i);
      } 
  }

  
  printf("%d\n", nvehic);
  for (int i(0); i < nvehic; ++i) {
    Car &c = cars[i];
    printf("%d\n", c.path.size());
    for (auto i : c.path) {
      printf("%d\n", i);
    }
  }
}
