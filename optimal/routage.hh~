#ifndef __ROUTAGE_HH
#define __ROUTAGE_HH

#include "intersection.hh"
#include "road.hh"
#include "car.hh"
#include <utility>
#include <algorithm>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/hawick_circuits.hpp>

using namespace boost;
using namespace std;

typedef adjacency_list<vecS, vecS, bidirectionalS, no_property,
		       property< edge_weight_t, long, property< edge_weight2_t, long> >> Graph;

class Routage {
public:
  Routage();
  void parse(FILE* stream);
  void init(); // TODO

private:
  // Graph prepare_edges();
  Intersection* start;
  Intersection* inters;
  long ninters;
  Road* roads;
  long nroads;
  Car* cars;
  long ncars;
  pair<long, long>* edges;
  long nedges;
  long time;
};

#endif // __ROUTAGE_HH
