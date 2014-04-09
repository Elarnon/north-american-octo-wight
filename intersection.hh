#ifndef __INTERSECTION_HH
#define __INTERSECTION_HH

struct Car;
struct Road;

class Intersection {
public:
  Intersection();
  Intersection(long p_id, double p_lat, double p_lon);

  long id;
  double lat;
  double lon;
  long color;
  // TODO
};

#endif // __INTERSECTION_HH
