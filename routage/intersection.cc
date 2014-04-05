#include "main.h"

void Intersection::set(Intersection** inters, long ninters, Rue** rues, long nrues, long id, double lat, double lon) {
  this->inters = inters;
  this->ninters = ninters;
  this->rues = rues;
  this->nrues = nrues;
  this->id = id;
  this->lat = lat;
  this->lon = lon;
}
