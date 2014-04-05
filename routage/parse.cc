#include "main.h"
#include <cstdio>

void parse() {
  scanf(" %d %d %d %d %d", &ninters, &nrues, &time, &nvehic, &start);
  Intersection* inters = new Intersection[ninters];
  Rue* rues = new Rue[nrues];
  for (int inter(0); inter < ninters; ++inter) {
    double lat, lon;
    scanf(" %lf %lf", &lat, &lon);
    inters[inter].set(inter, lat, lon);
  }
  for (int ru(0); ru < nrues; ++ru) {
    long start, end, bidir, cost, score;
    scanf(" %d %d %d %d %d", &start, &end, &bidir, &cost, &score);
    rues[ru].set(ru, start, end, bidir == 2, cost, score);
  }
  Car* cars = new Car[nvehic];
  for (int i(0); i < nvehic; ++i) {
    cars[i].set(start);
  }
}
