#include "main.h"
#include <queue>
#include <cstdio>

using namespace std;

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

void dump(FILE* f) {
  fprintf(f, "%ld\n", nvehic);
  for (long i(0); i < nvehic; ++i) {
    fprintf(f, "%ld\n", cars[i].path.size());
    for (long j : cars[i].path) {
      fprintf(f, "%ld\n", j);
    }
  }
}

int main(int argc, char* argv[]) {
  parse();
  FILE* f(fopen(argv[1], "r"));
  long check;
  fscanf(f, " %ld", &check);
  for (long i(0); i < nvehic; ++i) {
    Car &c(cars[i]);
    long nmoves, dummy;
    fscanf(f, " %ld %ld", &nmoves, &dummy);
    for (long j(1); j < nmoves; ++j) {
      long inter;
      fscanf(f, " %ld", &inter);
      bool ok = false;
      for (Rue* r : inters[c.pos].alls) {
	if (r->path(c.pos) == inter) {
	  c.move(inter, r->time);
	  ok = true;
	  break;
	}
      }
    }
  }
  fclose(f);
  dump(stdout); 
}
