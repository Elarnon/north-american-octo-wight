#include "main.h"

void Rue::set(Intersection** inters, long ninters, Rue** rues, long nrues, long id, long start, long end, bool bidir, long cost, long score) {
  this->inters = inters;
  this->ninters = ninters;
  this->rues = rues;
  this->nrues = nrues;
  this->id = id;
  this->start = start;
  this->end = end;
  this->bidir = bidir;
  this->time = cost;
  this->score = score;
  this->ok = false;
}


long Rue::path(long start) {
  if (start == this->start) {
    return this->end;
  } else if (start = this-> end && bidir) {
    return this->start;
  } else {
    return -1;
  }
}

std::pair<long, long> Rue::raw() {
  if (this->ok) {
    return std::make_pair(0, this->time);
  } else {
    return std::make_pair(this->score, this->time);
  }
}

std::pair<long, long> Rue::real_gain(long pos, long k) {
  // TODO
  return this->raw();
}

std::pair<long, long> Rue::gain(long pos) {
  // TODO
  return this->raw();
}
