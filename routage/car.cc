#include "main.h"

Car::Car(Intersection** inters, long ninters, Rue** rues, long nrues, long pos) {
  this->inters = inters;
  this->ninters = ninters;
  this->rues = rues;
  this->nrues = nrues;
  this->pos = pos;
  this->time = 0;
}

void Car::move(long npos, long cost) {
  this->pos = npos;
  this->time = this->time + cost;
  this->path.push_back(npos);
}
