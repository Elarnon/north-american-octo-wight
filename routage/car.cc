#include "main.h"

void Car::set(long pos) {
  this->pos = pos;
  this->time = 0;
}

void Car::move(long npos, long cost) {
  this->pos = npos;
  this->time = this->time + cost;
  this->path.push_back(npos);
}
