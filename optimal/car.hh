#ifndef __CAR_HH
#define __CAR_HH

#include <list>

using namespace std;

class Intersection;
class Road;

class Car {
public:
  Car();
  Car(long p_id, Intersection* p_pos);
  bool operator<(const Car& other);

  void move(Road* road);
  void infos(long time);
  void recompute(Intersection *start);

  long id;
  Intersection* pos;
  long time;
  long want;
  list<Road*> path;
};

#endif // __CAR_HH
