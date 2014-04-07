#ifndef __ROAD_HH
#define __ROAD_HH

struct Intersection;
struct Car;

class Road {
public:
  Road();
  Road(long id, Intersection *start, Intersection *end, bool bidir, long time, long score);

  void colorize();

  long id;
  Intersection* start;
  Intersection* end;
  bool bidir;
  long time;
  long score;
  bool done;
  long color;
  // TODO
};

#endif // __ROAD_HH
