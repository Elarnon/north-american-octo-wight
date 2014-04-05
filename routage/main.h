#ifndef MAIN_HEADER_GUARD
#define MAIN_HEADER_GUARD

#define PROFONDEUR 5

#include<vector>
#include<list>
#include<unordered_set>
#include <cstdio>
#include <algorithm>

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

  std::pair<long, long> raw(std::unordered_set<Rue*> &visited);
  std::vector<std::pair<long, long>> real_gain(long pos, long k, std::unordered_set<Rue*> &visited);
  double gain(long pos);
};

extern Intersection *inters;
extern long ninters;
extern Rue *rues;
extern long nrues;
extern Car *cars;
extern long nvehic;
extern long start;
extern long total_time;
extern long *dist;

void parse();

void djikstra_init();
void dijkstra(long from);
std::list<long> find_path(long to);
std::list<long> find_one_path(std::vector<long> tos);

#endif
