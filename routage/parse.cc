#include "main.h"
#include <cstdio>

Routage* parse() {
  long ninter, nrues, time, nvehic, start;
  scanf(" %d %d %d %d %d", &ninter, &nrues, &time, &nvehic, &start);
  Intersection** inters = new Intersection*[ninter];
  Rue** rues = new Rue*[nrues];
  for (int inter(0); inter < ninter; ++inter) {
    double lat, lon;
    scanf(" %lf %lf", &lat, &lon);
    inters[inter] = new Intersection(inters, ninter, rues, nrues, inter, lat, lon);
  }
  for (int ru(0); ru < nrues; ++ru) {
    long start, end, bidir, cost, score;
    scanf(" %d %d %d %d %d", &start, &end, &bidir, &cost, &score);
    rues[ru] = new Rue(inters, ninter, rues, nrues, ru, start, end, bidir == 2, cost, score);
  }
  Car** cars = new Car*[nvehic];
  for (int i(0); i < nvehic; ++i) {
    cars[i] = new Car(inters, ninter, rues, nrues, start);
  }
  return new Routage(inters, ninter, rues, nrues, cars, nvehic, start, time);
}
