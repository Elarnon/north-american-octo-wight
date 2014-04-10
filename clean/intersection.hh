#ifndef __INTERSECTION_HH
#define __INTERSECTION_HH

#include <unordered_set>
using namespace std;

struct Car;
struct Road;

class Intersection {
public:
  Intersection();
  Intersection(long p_id, double p_lat, double p_lon);

  bool is_deadend();

  long id;
  double lat;
  double lon;
  unordered_set<Road*> starts_here;
  unordered_set<Road*> ends_here;
  // TODO
};

#endif // __INTERSECTION_HH
