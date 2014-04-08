#ifndef __ROUTAGE_HH
#define __ROUTAGE_HH

#include "intersection.hh"
#include "road.hh"
#include "car.hh"
#include <utility>
#include <set>
#include <algorithm>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/astar_search.hpp>

using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, bidirectionalS, no_property,
		       property< edge_weight_t, long, property< edge_weight2_t, long> > > Graph;
struct location {
double x;
double y;
};

class Routage {
public:
  Routage();
  void parse(FILE* stream);
  void parse_sol(FILE* stream);
  void print(FILE* stream);
  void do_stuff();
  void prune();
  list<Road*>::iterator prunable(list<Road*>::iterator it, list<Road*>::iterator end);
  pair<Intersection*, list<Road*> > extend_path(Intersection* pos, Road* r);
  void correct();
  void search();
  void take_deadends();
  void make_graph();
  void elarnon();
  pair<list<Road*>, double> shortest_path(Intersection* from, Intersection* to);
  // TODO void search();

private:
  // Graph prepare_edges();
  Graph g;
  Intersection* start;
  Intersection** inters;
  long ninters;
  Road** roads;
  long nroads;
  Car** cars;
  long ncars;
  pair<long, long>* edges;
  long nedges;
  location* locs;
  long time;
};

#endif // __ROUTAGE_HH
