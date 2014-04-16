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
  void take(Road* r);
  void prune(bool swap);
  void kill();
  void collect(Car* c);
  void correct();
  void take_deadends();
  pair<list<Road*>, double> shortest_path(Intersection* from, Intersection* to);
  void shortest_paths_from(Intersection* from);
  pair<list<Road*>, long> shortest_path_to(Intersection* to);
  void shortest_paths_to(Intersection* to);
  pair<list<Road*>, long> shortest_path_from(Intersection* from);
  void search(long depth);
  void untangle();
  void cut();
  void cut(Car* from);
  long eval_cut(Car* from);
  void balance();
  void swap();
  void relax();
  void compute_shortest_paths();
  void dump_mpp(FILE* stream);
  void average();
  void multi_elarnon();
  void stat_last_prune();
  void make_graph();
  void multi_elarnon1(Car* slow, Car* fast);
  void elarnon1();
  void elarnon(long depth);
  // TODO void search();

private:
  // Graph prepare_edges();
  Graph g;
  Graph r;
  vector<Graph::vertex_descriptor> preds;
  vector<long> dists;
  vector<Graph::vertex_descriptor> succs;
  vector<long> rdists;
  Intersection* start;
  Intersection** inters;
  long ninters;
  Road** roads;
  long nroads;
  Car** cars;
  long ncars;
  pair<long, long>* edges;
  pair<long, long>* redges;
  long nedges;
  location* locs;
  long time;
};

#endif // __ROUTAGE_HH
