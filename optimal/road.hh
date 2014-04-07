#ifndef __ROAD_HH
#define __ROAD_HH

#include<list>
#include<unordered_set>

using namespace std;

struct Intersection;
struct Car;

class Road {
public:
  Road();
  Road(long id, Intersection *start, Intersection *end, bool bidir, long time, long score);

  Intersection* use(Intersection* from);

  double gain(Intersection* pos);

  long id;
  Intersection* start;
  Intersection* end;
  bool bidir;
  long time;
  long score;
  long done;
  // TODO
private:
  pair<long, long> raw(unordered_set<Road*> &visited);
  list<pair<long, long>> real_gain(Intersection* pos, long k, unordered_set<Road*> &visited);
};

#endif // __ROAD_HH
