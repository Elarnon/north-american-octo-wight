#ifndef MAIN_HEADER_GUARD
#define MAIN_HEADER_GUARD


#include<list>

struct Intersection;
struct Rue;

struct Car {
  Car(Intersection** inters, long ninters, Rue** rues, long nrues, long pos);
  void move(long npos, long cost);

  Intersection** inters;
  long ninters;
  Rue** rues;
  long nrues;
  long pos;
  long time;
  std::list<long> path;
};

struct Intersection {
  Intersection(Intersection** inters, long ninters, Rue** rues, long nrues, long id, double lat, double lon);

  Intersection** inters;
  long ninters;
  Rue** rues;
  long nrues;
  long id;
  double lat;
  double lon;
  std::list<Rue*> alls;
};

struct Rue {
  Rue(Intersection** inters, long ninters, Rue** rues, long nrues, long id, long start, long end, bool bidir, long cost, long score);

  Intersection** inters;
  long ninters;
  Rue** rues;
  long nrues;
  long id;
  long start;
  long end;
  bool bidir;
  long time;
  long score;
  bool ok;

  long path(long start);

  std::pair<long, long> raw();
  std::pair<long, long> real_gain(long pos, long k);
  std::pair<long, long> gain(long pos);
};

struct Routage {
  Routage(Intersection** inters, long ninters, Rue** rues, long nrues, Car** cars, long ncars, long start, long time);

  Intersection** inters;
  long ninters;
  Rue** rues;
  long nrues;
  Car ** cars;
  long ncars;
  long start;
  long time;
};


#endif
