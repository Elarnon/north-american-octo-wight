#include "intersection.hh"

Intersection::Intersection() {
}

Intersection::Intersection(long p_id, double p_lat, double p_lon) :
  id(p_id), lat(p_lat), lon(p_lon) {
  starts_here = {};
  ends_here = {};
}

bool Intersection::is_deadend() {
  return ends_here.size() <= 1;
}
