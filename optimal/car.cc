#include "car.hh"
#include "road.hh"
#include "intersection.hh"

Car::Car() {
}

Car::Car(long p_id, Intersection *p_pos)
  : id(p_id), pos(p_pos), want(0), time(0) {
}

void Car::recompute(Intersection* start) {
  printf("Recomputing...\n");
  time = 0;
  for (Road* r : this->path) {
    start = r->use(start);
    time += r->time;
  }
  pos = start;
  printf("Ok.\n");
}

void Car::move(Road* road) {
  path.push_back(road);
  pos = road->use(pos);
  road->done++;
  time += road->time;
}

void Car::infos(long t) {
  printf("Car #%ld: %ld time remaining (wants %ld)\n", id+1, t - time, want);
}

bool Car::operator<(const Car& other) {
  return time < other.time;
}
