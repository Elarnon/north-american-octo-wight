#include "main.h"

void Rue::set(long id, long a, long b, bool bidir, long cost, long score) {
  this->id = id;
  this->start = a;
  this->end = b;
  this->bidir = bidir;
  this->time = cost;
  this->score = score;
  this->ok = false;
  inters[start].alls.push_back(this);
  if (bidir) {
    inters[end].alls.push_back(this);
  }
}


long Rue::path(long start) {
  if (start == this->start) {
    return this->end;
  } else if (start == this->end && bidir) {
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

std::vector<std::pair<long, long>> Rue::real_gain(long pos, long k) {
  // TODO
  return { this->raw() };
}

double Rue::gain(long pos) {
  // TODO
  return 0.0d;
}
