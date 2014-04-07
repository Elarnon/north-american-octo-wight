#include "road.hh"
#include "intersection.hh"

Road::Road() {
}

Road::Road(long p_id, Intersection *p_start, Intersection *p_end, bool p_bidir, long p_time, long p_score) :
  id(p_id), start(p_start), end(p_end), bidir(p_bidir), time(p_time), score(p_score), done(false), color(0) {
}

void Road::colorize() {
  color = start->color;
}
