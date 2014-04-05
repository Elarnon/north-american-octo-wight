#include "main.h"

void Intersection::set(long id, double lat, double lon) {
  this->id = id;
  this->lat = lat;
  this->lon = lon;
  this->alls = std::list<Rue*>();
}
