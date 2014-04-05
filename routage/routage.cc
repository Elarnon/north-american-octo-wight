#include "main.h"

Routage::Routage(Intersection** inters, long ninters, Rue** rues, long nrues, Car** cars, long ncars, long start, long time) {
  this->inters = inters;
  this->ninters = ninters;
  this->rues = rues;
  this->nrues = nrues;
  this->cars = cars;
  this->ncars = ncars;
  this->start = start;
  this->time = time;
}
