#include "main.h"
#include <cstdio>

void parse() {
  scanf(" %ld %ld %ld %ld %ld", &ninters, &nrues, &total_time, &nvehic, &start);
  inters = new Intersection[ninters];
  rues = new Rue[nrues];
  cars = new Car[nvehic];
  for (int inter(0); inter < ninters; ++inter) {
    double lat, lon;
    scanf(" %lf %lf", &lat, &lon);
    inters[inter] = Intersection();
    inters[inter].set(inter, lat, lon);
  }
  for (int ru(0); ru < nrues; ++ru) {
    long a, b, bidir, cost, score;
    scanf(" %ld %ld %ld %ld %ld", &a, &b, &bidir, &cost, &score);
    rues[ru] = Rue();
    rues[ru].set(ru, a, b, bidir == 2, cost, score);
  }
  for (int i(0); i < nvehic; ++i) {
    cars[i] = Car();
    cars[i].set(start);
  }
}
