#ifndef MAIN_HEADER_GUARD
#define MAIN_HEADER_GUARD

#include<vector>
#include<list>
#include <cstdio>

struct Intersection;
struct Rue;

struct Car {
  void set(long pos);
  void move(long npos, long cost);
  bool operator<(const Car& other);

  long pos;
  long time;
  std::list<long> path;
};

struct Intersection {
  void set(long id, double lat, double lon);

  long id;
  double lat;
  double lon;
  std::list<Rue*> alls;
};

struct Rue {
  void set(long id, long start, long end, bool bidir, long cost, long score);

  long id;
  long start;
  long end;
  bool bidir;
  long time;
  long score;
  bool ok;

  long path(long start);

  std::pair<long, long> raw();
  std::vector<std::pair<long, long>> real_gain(long pos, long k);
  double gain(long pos);
};

extern Intersection *inters;
extern long ninters;
extern Rue *rues;
extern long nrues;
extern Car *cars;
extern long nvehic;
extern long start;
extern long time;

void parse();

#endif
