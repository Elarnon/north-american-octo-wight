#include "routage.hh"
#include <boost/graph/graphviz.hpp>
#include <iostream>

struct location {
  double lat, lon;
};

template <class Graph, class CostType, class LocMap>
class distance_heuristic : public astar_heuristic<Graph, CostType>
{
public:
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  distance_heuristic(LocMap l, Vertex goal)
    : m_location(l), m_goal(goal) {}
  CostType operator()(Vertex u)
  {
    CostType dx = m_location[m_goal].x - m_location[u].x;
    CostType dy = m_location[m_goal].y - m_location[u].y;
    return ::sqrt(dx * dx + dy * dy);
  }
private:
  LocMap m_location;
  Vertex m_goal;
};

struct found_goal {}; // exception for termination

// visitor that terminates when we find the goal
template <class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor
{
public:
  astar_goal_visitor(Vertex goal) : m_goal(goal) {}
  template <class Graph>
  void examine_vertex(Vertex u, Graph& g) {
    if(u == m_goal)
      throw found_goal();
  }
private:
  Vertex m_goal;
};

Routage::Routage()
  : start(NULL), edges(NULL), nedges(0), inters(NULL), ninters(0)
  , roads(NULL), nroads(0), cars(NULL), ncars(0) {
}

void Routage::parse(FILE* stream) {
  delete[] inters;
  delete[] roads;
  delete[] cars;
  long start_pos;
  fscanf(stream, " %ld %ld %ld %ld %ld", &ninters, &nroads, &time, &ncars, &start_pos);
  inters = new Intersection[ninters];
  roads = new Road[nroads];
  cars = new Car[ncars];
  double min_lat(0.0d), max_lat(0.0d);
  double min_lon(0.0d), max_lon(0.0d);
  for (long i(0); i < ninters; ++i) {
    double lat, lon;
    fscanf(stream, " %lf %lf", &lat, &lon);
    if (lat < min_lat) {
      min_lat = lat - 1.0d;
    }
    if (lon < min_lon) {
      min_lon = lon - 1.0d;
    }
    if (lat > max_lat) {
      max_lat = lat + 1.0d;
    }
    if (lon > max_lon) {
      max_lon = lon + 1.0d;
    }
    auto inter(Intersection(i, lat, lon));
    inters[i] = inter;
    if (i == start_pos) {
      start = &inter;
    }
  }
  for (long i(0); i < nroads; ++i) {
    long a, b, bidir, cost, score;
    fscanf(stream, " %ld %ld %ld %ld %ld", &a, &b, &bidir, &cost, &score);
    roads[i] = Road(i, &inters[a], &inters[b], bidir == 2, cost, score);
  }
  for (long i(0); i < ncars; ++i) {
    cars[i] = Car(i, start);
  }
  long sgran = 10;
  auto color_map = new unordered_set<Rue*>[sgran * sgran];
  for (long i(0); i < ninters; ++i) {
    long latid(static_cast<long>((sgran * (ninters->lat - min_lat)) / (max_lat - min_lat)));
    long lonid(static_cast<long>((sgran * (ninters->lon - min_lon)) / (max_lon - min_lon)));
    if (latid % 2 == 0) {
      inters[i].color = latid * sgran + lonid;
    } else {
      inters[i].color = latid * sgran + (sgran - lonid);
    }
  }
  for (long i(0); i < nroads; ++i) {
    Road& road(roads[i]);
    road.colorize();
    color_map[road.color].insert(&road);
  }
}

Graph Routage::prepare_edges() {
  nedges = 0;
  for (auto road : roads) {
    nedges += (road->bidir ? 2 : 1);
  }
  delete[] edges;
  edges = new pair<long, long>[nedges];
  long weights[nedges];
  long i(0);
  for (auto road : roads) {
    edges[i].first = road->start->id;
    edges[i].second = road->end->id;
    weights[i] = road->time;
    ++i;
    // TODO: map[road->start->id][road->end->id] = road;
    if (road->bidir) {
      edges[i].first = road->end->id;
      edges[i].second = road->start->id;
      weights[i] = road->time;
      ++i;
      // TODO: map[road->end->id][road->start->id] = road;
    }
  }
  auto g = Graph(edges, edges + nedges, inters.size());
  property_map < Graph, edge_weight_t >::type w = get(edge_weight, g);
  graph_traits < Graph >::edge_iterator e, e_end;
  i = 0;
  for (boost::tie(e, e_end) = boost::edges(g); e != e_end; ++e) {
    w[*e] = weights[i++];
  }
  return g;
}

void Routage::init() {
  /*
  Graph g(prepare_edges());
  std::map<long unsigned int, long unsigned int> m;
  boost::associative_property_map< std::map<long unsigned int, long unsigned int> > doms(m);
  unordered_set< pair<long unsigned int, long unsigned int> > res;
  long **D = new long*[inters.size()];
  for (long i(0); i < inters.size(); ++i) {
    D[i] = new long[inters.size()];
  }
  lengauer_tarjan_dominator_tree(g, start->id, doms);
  typedef graph_traits < Graph >::edge_descriptor Edge;
  std::vector < Edge > spanning_tree;
  kruskal_minimum_spanning_tree(g, std::back_inserter(spanning_tree));
  // johnson_all_pairs_shortest_paths(g, D);
  
  long sources[inters.size()];
  long targets[inters.size()];
  for (long i(0); i < inters.size(); ++i) {
    sources[i] = 0;
    targets[i] = 0;
  }
  graph_traits<Graph>::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei) {
    sources[source(*ei, g)]++;
    targets[target(*ei, g)]++;
  }
  long ns = 0;
  for (long i(0); i < inters.size(); ++i) {
    if (sources[i] == 1 && targets[i] == 1) {
      ns += 1;
    }
  }
  printf("Useless vertices: %d\n", ns);
  long nb = 0;
  long no = 0;
  for (auto& p : m) {
    if (p.second == start->id) {
      nb += 1;
    } else {
      no += 1;
    }
  }
  printf("Children of start: %d\n", nb);
  printf("Others:            %d\n", no);*/
}
