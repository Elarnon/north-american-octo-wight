#include "routage.hh"
#include <iostream>
#include <cstdlib>

#include <queue>
#include <unordered_set>
#include <boost/graph/dijkstra_shortest_paths.hpp>

template<class T> bool is_before(typename list<T>::iterator x, typename list<T>::iterator y, typename list<T>::iterator e) {
  auto x_nxt = x;
  auto y_nxt = y;
  while (x_nxt != y && y_nxt != x) {
    if (x_nxt != e) {
      x_nxt = boost::next(x_nxt);
    }
    if (y_nxt != e) {
      y_nxt = boost::next(y_nxt);
    }
  }
  return (x_nxt == y);
}

Routage::Routage()
  : start(NULL), edges(NULL), nedges(0), inters(NULL), ninters(0)
  , roads(NULL), nroads(0), cars(NULL), ncars(0), locs(NULL)
  , redges(NULL) {
}

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

void Routage::parse(FILE* stream) {
  delete[] inters;
  delete[] roads;
  delete[] cars;
  long start_pos;
  fscanf(stream, " %ld %ld %ld %ld %ld", &ninters, &nroads, &time, &ncars, &start_pos);
  inters = new Intersection*[ninters];
  roads = new Road*[nroads];
  cars = new Car*[ncars];
  double min_lat(0.0d), max_lat(0.0d);
  double min_lon(0.0d), max_lon(0.0d);
  for (long i(0); i < ninters; ++i) {
    double lat, lon;
    fscanf(stream, " %lf %lf", &lat, &lon);
    auto inter(new Intersection(i, lat, lon));
    inters[i] = inter;
    if (i == start_pos) {
      start = inter;
    }
  }
  for (long i(0); i < nroads; ++i) {
    long a, b, bidir, cost, score;
    fscanf(stream, " %ld %ld %ld %ld %ld", &a, &b, &bidir, &cost, &score);
    roads[i] = new Road(i, inters[a], inters[b], bidir == 2, cost, score);
  }
  for (long i(0); i < ncars; ++i) {
    cars[i] = new Car(i, start);
  }
}

void Routage::dump_mpp(FILE* stream) {
  fprintf(stream, "NUMNODES %ld\n", ninters);
  long arcs(0);
  for (long i(0); i < nroads; ++i) {
    if (!roads[i]->bidir) {
      arcs++;
    }
  }
  fprintf(stream, "NUMARCS %ld\n", arcs);
  fprintf(stream, "NUMEDGES %ld\n", nroads - arcs);
  for (long i(0); i < nroads; ++i) {
    if (!roads[i]->bidir) {
      fprintf(stream, "A %ld %ld %ld\n", roads[i]->start->id, roads[i]->end->id, roads[i]->time);
    }
  }
  for (long i(0); i < nroads; ++i) {
    if (roads[i]->bidir) {
      fprintf(stream, "E %ld %ld %ld\n", roads[i]->start->id, roads[i]->end->id, roads[i]->time);
    }
  }
}

void Routage::average() {
  long mini(0), min(cars[0]->time);
  for (int i(0); i < ncars; ++i) {
    if (cars[i]->time > min) {
      mini = i;
      min = cars[i]->time;
    }
  }
  printf("Chose mini: %ld\n", mini);
  for (int j(0); j < ncars; ++j) {
    if (j == mini) continue;
    long x(0);
    boost::unordered_set<Road*> iroads;
    for (auto it(cars[mini]->path.begin()); it != cars[mini]->path.end(); ++it) {
      iroads.insert(*it);
    }
    Intersection *pos(start);
    for (auto jt(cars[j]->path.begin()); jt != cars[j]->path.end(); ++jt) {
      // pos - jt ->
      auto iit = iroads.find(*jt);
      if (iit != iroads.end()) {
	Intersection *spos(start);
	list<Road*>::iterator istart;
	for (auto tmp(cars[mini]->path.begin()); tmp != cars[mini]->path.end(); ++tmp) {
	  istart = tmp;
	  if (*tmp == *iit && spos == pos) {
	    break;
	  }
	  spos = (*tmp)->use(spos);
	}
	if (spos == pos) {
	  // spos==pos - istart ->
	  Intersection *opos(pos);
	  long jtime(0);
	  for (auto tmp(jt); tmp != cars[j]->path.end(); ++tmp) {
	    // pos - jt - .. -> opos - tmp ->
	    if (iroads.find(*tmp) != iroads.end()) {
	      list<Road*>::iterator sol = cars[mini]->path.end();
	      long itime(0);
	      Intersection *ospos(spos);
	      // spos==pos - istart -.. - ospos - sol ->
	      for (auto imp(istart); imp != cars[mini]->path.end(); ++imp) {
		sol = imp;
		if (ospos == opos) break;
		ospos = (*imp)->use(ospos);
		itime += (*imp)->time;
	      }
	      if (sol == cars[mini]->path.end() || cars[mini]->time - itime + jtime > time ||
		  cars[j]->time - jtime + itime >= min || itime <= jtime
		  || tmp == jt || sol == istart || ospos != opos) {
		// Nope.
		opos = (*tmp)->use(opos);
		jtime += (*tmp)->time;
		continue;
	      } else {
		printf("Balancing %ld ~ %ld\n", jtime, itime);
		// Yes
		printf("[%ld] %ld - istart -> %ld - .. -> %ld - sol ->\n", mini, spos->id, (*istart)->use(spos)->id, ospos->id);
		printf("%ld and %ld for %ld\n", (*jt)->start->id, (*jt)->end->id, pos->id);
		printf("[%ld] %ld - jt -> %ld - .. -> %ld - tmp ->\n", j, pos->id, (*jt)->use(pos)->id, opos->id);
		cars[mini]->path.splice(istart, cars[j]->path, jt, tmp);
		cars[j]->path.splice(tmp, cars[mini]->path, istart, sol);
		cars[mini]->time = cars[mini]->time - itime + jtime;
		cars[j]->time = cars[j]->time - jtime + itime;
		return;
	      }
	    }
	    opos = (*tmp)->use(opos);
	    jtime += (*tmp)->time;
	  }
	}
      }
      pos = (*jt)->use(pos);
    }
  }
}

void Routage::swap() {
  long done(-1);
  long moved(0);
  for (long i(rand() % ncars); i < ncars; ++i) {
    long max(cars[0]->time);
    for (long j(0); j < ncars; ++j) {
      if (cars[j]->time > max) {
	max = cars[j]->time;
      }
    }
    if (max < 5900) {
      printf("Stop %ld %ld\n", max, moved);
      break;
    }
    elarnon1();
    prune(false);
    correct();
    multi_elarnon();
    do_stuff();
    Intersection *pos(start);
    long t(0);
    unordered_map<Intersection*, list<Road*>::iterator> spl = {};
    for (auto it(cars[i]->path.begin()); it != cars[i]->path.end(); ++it) {
      t += (*it)->time;
      spl.insert({ pos, it });
      pos = (*it)->use(pos);
    }
    bool stop(false);
    for (long j(0); j < ncars; ++j) {
      if (stop) {
	break;
      }
      if (i == j) continue;
      Intersection *opos(start);
      for (auto it(cars[j]->path.begin()); it != cars[j]->path.end(); ++it) {
	auto f = spl.find(opos);
	if (f != spl.end() && rand() % 10 == 3) {
	  list<Road*>::iterator pt;
	  boost::tie(pos, pt) = *f;
	  long j_rm(0), i_rm(0);
	  for (auto at(it); at != cars[j]->path.end(); ++at) {
	    j_rm += (*at)->time;
	  }
	  for (auto at(pt); at != cars[i]->path.end(); ++at) {
	    i_rm += (*at)->time;
	  }
	  if (cars[i]->time - i_rm + j_rm <= time && cars[j]->time - j_rm + i_rm <= time) {
	    // printf("Swapping %ld in %ld with %ld in %ld\n", i_rm, i, j_rm, j);
	    cars[i]->path.splice(pt, cars[j]->path, it, cars[j]->path.end());
	    cars[j]->path.splice(cars[j]->path.end(), cars[i]->path, pt, cars[i]->path.end());
	    long tmpi = cars[i]->time - i_rm + j_rm;
	    long tmpj = cars[j]->time - j_rm + i_rm;
	    cars[i]->recompute(start);
	    cars[j]->recompute(start);
	    if (cars[i]->time != tmpi || cars[j]->time != tmpj) {
	      printf("Incoherent.\n");
	      exit(9);
	    }
	    done = i;
	    moved++;
	    if (moved / (i + 1) < 10) {
	      --i;
	    } else {
	      done = i - 1;
	    }
	    stop = true;
	    break;
	  }
	}
	opos = (*it)->use(opos);
      }
    }
  }
}

void Routage::parse_sol(FILE* stream) {
  long check;
  fscanf(stream, " %ld", &check);
  for (long i(0); i < ncars; ++i) {
    Car *c(cars[i]);
    long nmoves, dummy;
    fscanf(stream, " %ld %ld", &nmoves, &dummy);
    for (long j(1); j < nmoves; ++j) {
      long inter;
      fscanf(stream, " %ld", &inter);
      for (Road* r : c->pos->starts_here) {
	if (r->use(c->pos)->id == inter) {
	  c->move(r);
	  break;
	}
      }
    }
  }
}

void Routage::make_graph() {
  nedges = 0;
  for (long j(0); j < nroads; ++j) {
    nedges += (roads[j]->bidir ? 2 : 1);
  }
  delete[] redges;
  delete[] edges;
  delete[] locs;
  locs = new location[ninters];
  for (long j(0); j < ninters; ++j) {
    locs[j].x = inters[j]->lat;
    locs[j].y = inters[j]->lon;
  }
  edges = new pair<long, long>[nedges];
  redges = new pair<long, long>[nedges];
  long weights[nedges];
  long rweights[nedges];
  long i(0);
  for (long j(0); j < nroads; ++j) {
    Road* road(roads[j]);
    edges[i].first = road->start->id;
    edges[i].second = road->end->id;
    weights[i] = road->time;
    redges[i].first = road->end->id;
    redges[i].second = road->start->id;
    rweights[i] = road->time;
    ++i;
    // TODO: map[road->start->id][road->end->id] = road;
    if (road->bidir) {
      edges[i].first = road->end->id;
      edges[i].second = road->start->id;
      weights[i] = road->time;
      redges[i].first = road->start->id;
      redges[i].second = road->end->id;
      rweights[i] = road->time;
      ++i;
      // TODO: map[road->end->id][road->start->id] = road;
    }
  }
  g = Graph(edges, edges + nedges, ninters);
  property_map < Graph, edge_weight_t >::type w = get(edge_weight, g);
  graph_traits < Graph >::edge_iterator e, e_end;
  i = 0;
  for (boost::tie(e, e_end) = boost::edges(g); e != e_end; ++e) {
    w[*e] = weights[i++];
  }
  r = Graph(redges, redges + nedges, ninters);
  property_map < Graph, edge_weight_t >::type rw = get(edge_weight, r);
  i = 0;
  for (boost::tie(e, e_end) = boost::edges(r); e != e_end; ++e) {
    rw[*e] = rweights[i++];
  }
}

void Routage::print(FILE* stream) {
  fprintf(stream, "%ld\n", ncars);
  for (long i(0); i < ncars; ++i) {
    fprintf(stream, "%ld\n", 1 + cars[i]->path.size());
    Intersection* pos(start);
    fprintf(stream, "%ld\n", pos->id);
    for (Road* r : cars[i]->path) {
      pos = r->use(pos);
      fprintf(stream, "%ld\n", pos->id);
    }
  }
}

void Routage::shortest_paths_to(Intersection* to) {
  succs = vector<Graph::vertex_descriptor>(num_vertices(r));
  rdists = vector<long>(num_vertices(r));
  dijkstra_shortest_paths(r, vertex(to->id, r),
			  predecessor_map(boost::make_iterator_property_map(preds.begin(), get(boost::vertex_index, r))).
                          distance_map(boost::make_iterator_property_map(dists.begin(), get(boost::vertex_index, r))));
}

void Routage::shortest_paths_from(Intersection* from) {
  // What size in mem ?
  //auto *stuff = new vector<Graph::vertex_descriptor>(preds);
  //auto *staff = new vector<long>(dists);
  preds = vector<Graph::vertex_descriptor>(num_vertices(g));
  dists = vector<long>(num_vertices(g));
  dijkstra_shortest_paths(g, vertex(from->id, g),
			  predecessor_map(boost::make_iterator_property_map(preds.begin(), get(boost::vertex_index, g))).
                          distance_map(boost::make_iterator_property_map(dists.begin(), get(boost::vertex_index, g))));
}

pair<list<Road*>, long> Routage::shortest_path_to(Intersection* to) {
  list<Road*> sp = { };
  Intersection* cur(NULL);
  for (auto v = to->id;; v = preds[v]) {
    if (cur == NULL) {
      cur = inters[v];
    } else {
      Road* rue(NULL);
      for (auto r : inters[v]->starts_here) {
	if (r->use(inters[v]) == cur) {
	  rue = r;
	  break;
	}
      }
      sp.push_front(rue);
      cur = inters[v];
      if (preds[v] == v) {
	break;
      }
    }
  }
  return make_pair(sp, dists[to->id]);
}

pair<list<Road*>, long> Routage::shortest_path_from(Intersection* from) {
  list<Road*> sp = { };
  Intersection* cur(NULL);
  for (auto v = from->id;; v = succs[v]) {
    if (cur == NULL) {
      cur = inters[v];
    } else {
      Road* rue(NULL);
      for (auto r : inters[v]->ends_here) {
	if (r->use(inters[v]) == cur) {
	  rue = r;
	  break;
	}
      }
      sp.push_front(rue);
      cur = inters[v];
      if (succs[v] == v) {
	break;
      }
    }
  }
  return make_pair(sp, rdists[from->id]);
}

pair<list<Road*>, double> Routage::shortest_path(Intersection* from, Intersection* to) {
  if (from->id == to->id) {
    return { {}, 0.0d };
  }
  vector<Graph::vertex_descriptor> p(num_vertices(g));
  vector<double> d(num_vertices(g));
  try {
    astar_search(g, from->id,
		 distance_heuristic<Graph, double, location*>(locs, to->id),
    		 predecessor_map(&p[0]).
		 distance_map(&d[0]).
		 visitor(astar_goal_visitor<Graph::vertex_descriptor>(to->id)));
  } catch (found_goal fg) {
    list<Road*> sp = {};
    Intersection* cur(NULL);
    for (auto v = to->id;; v = p[v]) {
      if (cur == NULL) {
	cur = inters[v];
	if (p[v] == v) {
	  printf("WUT %ld %ld\n", from->id, to->id);
	  break;
	}
      } else {
	// find road - inters[v]->cur
	Road* rue(NULL);
	for (auto r : inters[v]->starts_here) {
	  if (r->use(inters[v]) == cur) {
	    rue = r;
	    break;
	  }
	}
	if (rue == NULL) {
	  exit(29);
	}
	sp.push_front(rue);
	cur = inters[v];
	if (p[v] == v) {
	  break;
	}
      }
    }
    /*
      printf("%ld", from->id);
      for (Road* r : sp) {
      printf(" = %ld -> %ld", r->start->id, r->end->id);
    }
    printf(" = %ld\n", to->id); */
    return make_pair(sp, d[to->id]);
  }
  exit(17);
}

void Routage::search(long depth) {
  auto pq = std::priority_queue<Car*, std::vector<Car*>>();
  for (long i(0); i < ncars; ++i) {
    pq.push(cars[i]);
  }
  int nb(0);
  while (nb < 1000 && !pq.empty()) {
    nb++;
    auto c(pq.top());
    pq.pop();
    Road *best(NULL);
    double best_cost = 0;
    for (Road *r : c->pos->starts_here) {
      if (r->time + c->time < time) {
	auto cst(r->gain(c->pos, depth));
	if (cst > -1.0) {
	  if (best == NULL || cst > best_cost) {
	    best_cost = cst;
	    best = r;
	  }
	}
      }
    }
    if (best != NULL) {
      if (best_cost < 0.01d) {
	collect(c);
      } else {
	c->move(best);
      }
      pq.push(c);
    }
  }
}

list<Road*>::iterator prunable(Car* c, list<Road*>::iterator it, list<Road*>::iterator end) {
  while (it != end && (*it)->done > 1) {
    (*it)->done--;
    c->time -= (*it)->time;
    ++it;
  }
  return it;
}

pair<Intersection*, list<Road*> > extend_path(Car* c, Intersection* pos, Road* r) {
  list<Road*> res;
  while (r->done == 0) {
    res.push_back(r);
    pos = r->use(pos);
    r->done++;
    c->time += r->time;
    for (Road *other : pos->starts_here) {
      if (other->done == 0) {
	r = other;
	break;
      }
    }
  }
  return make_pair(pos, res);
}

void Routage::take(Road* r) {
  for (int i(0); i < ncars; ++i) {
    Car *c = cars[i];
    list<Road*> path_to;
    double dist_to;
    Intersection *pos(start), *spos;
    bool prunable(false);
    list<Road*>::iterator sit;
    shortest_paths_from(r->start); //TODO: to
    for (auto it(c->path.begin()); it != c->path.end(); ++it) {
      if (prunable) {
	if ((*it)->done == 1) {
	  // No longer prunable.
	  prunable = false;
	  dist_to = dists[spos->id];
	  if (c->time + dist_to + r->time <= time) {
	    // Find end.
	    printf("Hey, maybe!\n");
	    exit(10);
	  } else {
	    for (auto at(sit); at != it; ++at) {
	      (*at)->done++;
	      c->time += (*at)->time;
	    }
	  }
	}
      } else {
	if ((*it)->done > 1) {
	  (*it)->done--;
	  c->time -= (*it)->time;
	  spos = pos;
	  sit = it;
	  prunable = true;
	}
      }
      pos = (*it)->use(pos);
    }
  }
}

void Routage::collect(Car* c) {
  shortest_paths_from(c->pos);
  list<Road*> missed;
  for (int i(0); i < nroads; ++i) {
    if (roads[i]->done == 0) {
      missed.push_front(roads[i]);
    }
  }
  bool first = true;
  double cur = 0.0d;
  list<Road*> best;
  for (Road* r : missed) {
    if (r->done > 0) {
      continue;
    }
    if (r->start == c->pos) {
      best = { r };
      cur = 0;
      break;
    } else {
      auto spd = shortest_path_to(r->start);
      if (first || spd.second < cur) {
	first = false;
	best = spd.first;
	cur = spd.second;
	best.push_back(r);
      }
    }
    if (r->bidir){
      if (r->end == c->pos) {
	best = { r };
	cur = 0;
	break;
      } else {
	auto spd = shortest_path_to(r->end);
	if (first || spd.second < cur) {
	  first = false;
	  best = spd.first;
	  best.push_back(r);
	  cur = spd.second;
	}
      }
    }
  }
  for (Road* r : best) {
    if (c->time + r->time < time) {
      c->move(r);
    } else {
      break;
    }
  }
}

void Routage::correct() {
  int nb_iters(0);
  for (int i(0); i < ncars; ++i) {
    Car* c = cars[i];
    Intersection* pos(start);
    bool restart(false);
    for (auto it(c->path.begin()), ie(c->path.end()); it != ie; ) {
      for (Road *other : pos->starts_here) {
	Intersection *nxt = other->use(pos);
	for (Road *oother : nxt->starts_here) {
	}
	if (other->done == 0) {
	  auto save = c->time;
	  auto tmp = extend_path(c, pos, other);
	  auto final = tmp.first;
	  auto path = tmp.second;
	  auto end = prunable(c, it, ie);
	  auto tpos = pos;
	  for (auto at(it); at != end; ++at) {
	    tpos = (*at)->use(tpos);
	  }
	  shortest_paths_from(final);
	  auto pd = shortest_path_to(tpos);
	  auto kewl = pd.first;
	  auto total = 0;
	  for (auto itt = kewl.begin(); itt != kewl.end(); ++itt) {
	    (*itt)->done++;
	    c->time += (*itt)->time;
	  }
	  if (c->time > time) {
	    long dist(0), lol(0);;
	    for (auto itt(path.begin()); itt != path.end(); ++itt) {
	      if ((*itt)->done == 1) {
		dist += (*itt)->score;
	      }
	      lol += (*itt)->time;
	    }
	    printf("Car #%ld would need %lds for %ldm (direct takes %ld)\n", i+1, c->time - time, dist, lol);
	    for (auto itt = kewl.begin(); itt != kewl.end(); ++itt) {
	      (*itt)->done--;
	      c->time -= (*itt)->time;
	    }
	    for (auto itt(path.begin()); itt != path.end(); ++itt) {
	      (*itt)->done--;
	      c->time -= (*itt)->time;
	    }
	    for (auto itt(it); itt != end; ++itt) {
	      (*itt)->done++;
	      c->time += (*itt)->time;
	    }
	    if (c->time != save) {
	      printf("Before: %d\n", save);
	      printf("After:  %d\n", c->time);
	    }
	  } else {
	    it = c->path.erase(it, end);
	    c->path.insert(it, path.begin(), path.end());
	    c->path.insert(it, kewl.begin(), kewl.end());
	    restart = true;
	    break;
	  }
	}
      }
      if (restart)
	break;
      pos = (*it)->use(pos);
      ++it;
    }
    if (restart)
      --i;
  }
}

void Routage::stat_last_prune() {
  for (int i(0); i < ncars; ++i) {
    Car *c = cars[i];
    Intersection *pos(c->pos);
    bool pruning = false;
    long t(0), d(0);
    for (auto it(c->path.rbegin()), ie(c->path.rend()); it != ie; ++it) {
      Road* r = *it;
      if (r->done == 1) {
	if (pruning && t > 4 * d) {// && 9 * t > 10 * d) {
	  printf("[%ld] Last prune would give %lds and lose %ldm.\n", i, t, d);
	  if (d == 0) {
	    for (auto ft(it.base()); ft != c->path.end(); ++ft) {
	      (*ft)->done--;
	      c->time -= (*ft)->time;
	    }
	    c->path.erase(it.base(), c->path.end());
	    break;
	  }
	}
	d += (*it)->score;
      } else {
	t += (*it)->time;
	pruning = true;
      }
      pos = r->use(pos);
    }
  }
}

void Routage::compute_shortest_paths() {
  for (int i(0); i < ninters; ++i) {
    shortest_paths_from(inters[i]);
    //write_shortest_paths();
  }
}

void Routage::prune(bool swap) {
  for (int i(0); i < ncars; ++i) {
    Car *c(cars[i]);
    pair<Intersection*, Intersection*> p;
    bool optimizing(false);
    Intersection* pos(start);
    long nprunable(0), t(0), d(0), tott(0);
    long cost(0);
    for (auto it(c->path.begin());
	 it != c->path.end(); ) {
      Intersection* oldpos = pos;
      pos = (*it)->use(pos);
      if ((*it)->done > 1) {// || ((*it)->done == 1 && cost + (*it)->score < 10 && (*it)->bidir)) {
	if ((*it)->done == 1) {
	  cost += (*it)->score;
	}
	if (optimizing) {
	  t += (*it)->time;
	  d += (*it)->score;
	  p.second = pos;
	} else {
	  t = (*it)->time;
	  d = (*it)->score;
	  optimizing = true;
	  p.first = oldpos;
	  p.second = pos;
	}
	(*it)->done--;
	c->time -= (*it)->time;
	it = c->path.erase(it);
      } else {
	if (optimizing && p.first != p.second) {
	  if (t > 500) {
	    printf("[%ld] Prunable path of %ldm (%lds). Consider swapping.\n", i, d, t);
	  }
	  if (t > 500 && swap) {
	    printf("[%ld] Prunable path of %ldm (%lds). Swapping.\n", i, d, t);
	    // p.first -b--it-> p.second must be deleted
	    // Search an entry to another car path near c->end
	    // such that they pass near p.second *later*
	    shortest_paths_from(c->pos);
	    boost::unordered_set<Intersection*> nears;
	    for (long j(0); j < ninters; ++j) {
	      if (dists[j] < 250) {
		nears.insert(inters[j]);
	      }
	    }
	    shortest_paths_from(p.second);
	    boost::unordered_set<Intersection*> near_p;
	    for (long j(0); j < ninters; ++j) {
	      if (dists[j] < 250) {
		near_p.insert(inters[j]);
	      }
	    }
	    for (long j(0); j < ncars; ++j) {
	      Intersection* cpos(start);
	      bool found(false);
	      for (Road* r : cars[j]->path) {
		if (near_p.find(cpos) != near_p.end()) {
		  found = true;
		}
		if (found && nears.find(cpos) != nears.end()) {
		  printf("Oh yeah !!!!\n");
		}
		cpos = r->use(cpos);
	      }
	    }
	  }
	  tott += t;
	  nprunable++;
	  auto tmp(shortest_path(p.first, p.second));
	  list<Road*> sp(tmp.first);
	  auto b(sp.begin());
	  c->path.splice(it, sp);
	  t = 0;
	  for (auto rit(b), rend(it); rit != rend; ++rit) {
	    t += (*rit)->time;
	    c->time += (*rit)->time;
	    (*rit)->done++;
	  }
	}
	++it;
	optimizing = false;
      }
    }
    printf("[%ld] Gain %lds maximum by pruning.\n", i, tott);
  }
}

void Routage::relax() {
  for (int i(0); i < ncars; ++i) {
    Car *c = cars[i];
    auto pre = c->path.begin();
    for (auto it(c->path.begin()); it != c->path.end(); ++it) {
      if (it == pre) continue;
      if (*it == *pre) {
	(*it)->done -= 2;
	c->time -= (*it)->time * 2;
	c->path.erase(pre, ++it);
	--it;
      }
      pre = it;
    }
  }
}

bool are_coprunables(list<Road*>::iterator as, list<Road*>::iterator ae,
		     list<Road*>::iterator bs, list<Road*>::iterator be) {
  bool res(true);
  for (auto at(as); at != ae; ++at) {
    if ((*at)->done <= 1) {
      res = false;
    }
    (*at)->done--;
  }
  for (auto bt(bs); bt != be; ++bt) {
    if ((*bt)->done <= 1) {
      res = false;
    }
    (*bt)->done--;
  }
  for (auto at(as); at != ae; ++at) {
    (*at)->done++;
  }
  for (auto bt(bs); bt != be; ++bt) {
    (*bt)->done++;
  }
  return res;
}

bool extract_coprunables(
			 list<pair<list<Road*>::iterator, list<Road*>::iterator>> &as,
			 list<pair<list<Road*>::iterator, list<Road*>::iterator>> &bs,
			 pair<list<Road*>::iterator, list<Road*>::iterator> &a,
			 pair<list<Road*>::iterator, list<Road*>::iterator> &b) {
  for (auto at(as.begin()), ae(as.end()); at != ae; ++at) {
    for (auto bt(bs.begin()), be(bs.end()); bt != be; ++bt) {
      // Check if path [at->first, at->second) and [bt->first, bt->second) are coprunables
      if (are_coprunables(at->first, at->second, bt->first, bt->second)) {
	a.first = at->first;
	a.second = at->second;
	b.first = bt->first;
	b.second = bt->second;
	as.erase(at);
	bs.erase(bt);
	return true;
      }
    }
  }
  return false;
}

void Routage::multi_elarnon1(Car *slow, Car* fast) {
  list<pair<list<Road*>::iterator, bool>> data[nroads];
  Intersection *pos(start);
  for (auto it(slow->path.begin()), e(slow->path.end()); it != e; ++it) {
    Road* r = *it;
    data[r->id].push_back(make_pair(it, pos == r->start));
    pos = r->use(pos);
  }
  pos = start;
  for (auto it(fast->path.begin()), e(fast->path.end()); it != e; ) {
    // pos -it-> npos
    bool dir = pos == (*it)->start;
    auto &previous = data[(*it)->id];
    bool baok(false), abok(false);
    list<pair<list<Road*>::iterator, bool>>::iterator ab, ba;
    for (auto pit(previous.begin()), pe(previous.end()); pit != pe; ++pit) {
      if (pit->second != dir) {
	baok = true;
	ba = pit;
      } else {
	abok = true;
	ab = pit;
      }
    }
    // pos -ab-> npos
    // npos -ba-> pos
    auto nxt = boost::next(it);
    if (abok && baok) {
      if (is_before<Road*>(ab->first, ba->first, slow->path.end())) {
	long dt;
	for (auto jit(boost::next(ab->first)); jit != ba->first; ++jit) {
	  dt += (*jit)->time;
	}
	if (fast->time + dt <= time) {
	  for (auto jit(boost::next(ab->first)); jit != ba->first; ++jit) {
	    slow->time -= (*jit)->time;
	    fast->time += (*jit)->time;
	  }
	  // pos -it-> npos { npos ... npos } npos
	  fast->path.splice(nxt, slow->path, boost::next(ab->first), ba->first);
	  (*ba->first)->done--;
	  slow->time -= (*ab->first)->time;
	  slow->time -= (*ba->first)->time;
	  slow->path.erase(ab->first);
	  slow->path.erase(ba->first);
	  break;
	}
      } else if (is_before<Road*>(ba->first, ab->first, slow->path.end())) {
	long dt;
	for (auto jit(boost::next(ba->first)); jit != ab->first; ++jit) {
	  dt += (*jit)->time;
	}
	if (fast->time + dt <= time) {
	  printf("We could!\n");
	}
      }
    }
    pos = (*it)->use(pos);
    it = nxt;
  }
}

void Routage::elarnon1() {
  list<pair<list<Road*>::iterator, bool>> data[nroads];
  for (long i(0); i < ncars; ++i) {
    for (long j(0); j < nroads; ++j) {
      data[j].clear();
    }
    Car *c = cars[i];
    Intersection* pos(start);
    for (auto it(c->path.begin()); it != c->path.end(); ) {
      Road *r = *it;
      bool dir = pos == r->start;
      pos = r->use(pos);
      auto &previous = data[r->id];
      bool baok(false), abok(false);
      list<pair<list<Road*>::iterator, bool>>::iterator ab, ba;
      for (auto pit(previous.begin()); pit != previous.end(); ++pit) {
	if (pit->second != dir) {
	  baok = true;
	  ba = pit;
	} else {
	  abok = true;
	  ab = pit;
	}
      }
      auto nxt = boost::next(it);
      if (abok && baok) {
	if (is_before<Road*>(nxt, ab->first, c->path.end()) || is_before<Road*>(nxt, ba->first, c->path.end())){
	  exit(7);
	}
	if (is_before<Road*>(ab->first, ba->first, c->path.end())) {
	  c->path.splice(nxt, c->path, ab->first, ba->first);
	  (*ab->first)->done--;
	  (*ba->first)->done--;
	  c->time -= (*ab->first)->time * 2;
	  c->path.erase(ab->first);
	  c->path.erase(ba->first);
	  previous.erase(ab);
	  previous.erase(ba);
	} else {
	  c->path.splice(it, c->path, ba->first, ab->first);
	  (*ab->first)->done--;
	  (*ba->first)->done--;
	  c->time -= (*ab->first)->time * 2;
	  c->path.erase(ab->first);
	  c->path.erase(ba->first);
	  previous.erase(ab);
	  previous.erase(ba);
	}
      }
      previous.push_back(make_pair(it, dir));
      it = nxt;
    }
  }
}

long Routage::eval_cut(Car* from) {
  shortest_paths_from(from->pos);
  long diff(99999999);
  for (long i(0); i < ncars; ++i) {
    if (cars[i] == from) continue;
    if (cars[i]->time > time) {
      Intersection *pos(start);
      auto mini(cars[i]->path.begin());
      Intersection* minpos(start);
      long min(-1);
      long t(0);
      long to_end(cars[i]->time);
      for (auto it(cars[i]->path.begin()), e(cars[i]->path.end()); it != e; ++it) {
	if ((min == -1 && t >= time / 2) || ((to_end > time / 2  || cars[i]->time - to_end <= time) && dists[pos->id] < min && from->time + to_end + dists[pos->id] <= time)) {
	  min = dists[pos->id];
	  mini = it;
	  minpos = pos;
	}
	t += (*it)->time;
	to_end -= (*it)->time;
	pos = (*it)->use(pos);
      }
      diff = from->time;
      if (minpos != from->pos) {
	auto p = shortest_path_to(minpos).first;
	for (Road* r : p) {
	  diff += r->time;
	}
      }
      long stuff(0);
      for (auto at(mini); at != cars[i]->path.end(); ++at) {
	stuff += (*at)->time;
      }
      /*if (from->time + diff + stuff > time) {
	return 99999999;
      }*/
      break;
    }
  }
  return diff;
}

void Routage::cut(Car* from) {
  shortest_paths_from(from->pos);
  for (long i(0); i < ncars; ++i) {
    if (cars[i] == from) continue;
    if (cars[i]->time > time) {
      Intersection *pos(start);
      auto mini(cars[i]->path.begin());
      Intersection* minpos(start);
      long min(-1);
      long t(0);
      long to_end(cars[i]->time);
      for (auto it(cars[i]->path.begin()), e(cars[i]->path.end()); it != e; ++it) {
	if ((min == -1 && t >= time / 2) || ((to_end > time / 2 || cars[i]->time - to_end <= time) && dists[pos->id] < min && from->time + to_end + dists[pos->id] <= time)) {
	  min = dists[pos->id];
	  mini = it;
	  minpos = pos;
	}
	t += (*it)->time;
	to_end -= (*it)->time;
	pos = (*it)->use(pos);
      }
      if (minpos != from->pos) {
	auto p = shortest_path_to(minpos).first;
	for (Road* r : p) {
	  from->move(r);
	}
      }
      for (auto at(mini); at != cars[i]->path.end(); ++at) {
	from->time += (*at)->time;
	cars[i]->time -= (*at)->time;
      }
      from->path.splice(from->path.end(), cars[i]->path, mini, cars[i]->path.end());
      break;
    }
  }
}

void Routage::cut() {
  long best(0);
  long cost(eval_cut(cars[0]));
  for (long i(1); i < ncars; ++i) {
    long tmp = eval_cut(cars[i]);
    printf("%ld/%ld for %ld\n", tmp, cost, i);
    if (tmp < cost) {
      cost = tmp;
      best = i;
    }
  }
  printf("Cutting %ld\n", best);
  cut(cars[best]);
}

void Routage::balance() {
  for (long i(0); i < ncars; ++i) {
    if (cars[i]->time > time) {
      // Find breaking point
      Intersection *pos(start);
      long t(0);
      list<Road*>::iterator fst;
      for (auto it(cars[i]->path.begin()), e(cars[i]->path.end()); it != e; ++it) {
	t += (*it)->time;
	if (t > time) {
	  fst = it;
	  break;
	}
	pos = (*it)->use(pos);
      }
      while (fst != cars[i]->path.end()) {
	if ((*fst)->done == 1) {
	  break;
	} else {
	  pos = (*fst)->use(pos);
	  (*fst)->done--;
	  cars[i]->time -= (*fst)->time;
	  fst = cars[i]->path.erase(fst);
	}
      }
      if (fst == cars[i]->path.end()) {
	continue;
      }
      long total(0);
      for (auto it(fst), e(cars[i]->path.end()); it != e; ++it) {
	total += (*it)->time;
      }
      bool first(true);
      double best_cost;
      list<Road*> best;
      long best_car;
      for (long j(0); j < ncars; ++j) {
	shortest_paths_from(cars[j]->pos);

	list<Road*> path;
	long cost;
	boost::tie(path, cost) = shortest_path_to(pos);
	if (first || (cost + cars[j]->time < best_cost && rand() % 3 == 2)) {
	  first = false;
	  best = path;
	  best_car = j;
	  best_cost = cost + cars[j]->time;
	}
      }
      if (!first) {
	cars[i]->time -= total;
	cars[best_car]->time += (best_cost - cars[best_car]->time) + total;
	cars[best_car]->path.insert(cars[best_car]->path.end(), best.begin(), best.end());
	cars[best_car]->path.splice(cars[best_car]->path.end(), cars[i]->path, fst, cars[i]->path.end());
	break;
      } else {
	printf("Unable to balance. Unfinished.\n");
	exit(39);
      }
    }
  }
}

void Routage::multi_elarnon() {
  bool first(true);
  for (long i(0); i < ncars; ++i) {
    for (long j(0); j < ncars; ++j) {
      if (j == i) continue;
      multi_elarnon1(cars[i], cars[j]);
    }
  }
  long maxi(cars[0]->time), mini(cars[0]->time);
  long imax(0), imin(0);
  for (long i(1); i < ncars; ++i) {
    if (cars[i]->time < mini) {
      imin = i;
      mini = cars[i]->time;
    }
    if (cars[i]->time > maxi) {
      imax = i;
      maxi = cars[i]->time;
    }
  }
  multi_elarnon1(cars[imax], cars[imin]);
}

void Routage::elarnon(long depth) {
  //elarnon1();
  for (long i(0); i < ncars; ++i) {
    printf("Car %ld...\n", i);
    bool restart = false;
    Car *c = cars[i];
    Intersection* pos(start);
    list<pair<Intersection*, list<Road*>::iterator>> buffer = {};
    unordered_map<long, pair<long, list<pair<list<Road*>::iterator, list<Road*>::iterator>>>> stuff = {};
    for (auto it(c->path.begin()); it != c->path.end(); ) {
      // pos - *it -> npos
      if ((*it)->use(pos) == NULL) {
	exit(17);
      }

      // For every path starting in the buffer and ending here
      for (auto tmp : buffer) {
	// Invariant: spos - *other -> .. pos - *it -> npos
	auto spos = tmp.first; // starting pos
	auto other = tmp.second; // iterator
	// abs are previous paths {spos -> pos}
	auto &abs = stuff[spos->id * ninters + pos->id];
	// bas are previous paths {pos -> spos}
	auto &bas = stuff[pos->id * ninters + spos->id];
	// In stuff[spos, pos] we have
	// spos - first -> .. pos -> last
	if (abs.first > 0 && bas.first > 0) {
	  // We may be able to optimize.
	  pair<list<Road*>::iterator, list<Road*>::iterator> abp, bap;
	  // Extract coprunable paths
	  if (extract_coprunables(abs.second, bas.second, abp, bap)) {
	    // Check if abp ends before bap starts
	    if (is_before<Road*>(abp.second, bap.first, c->path.end())) {
	      // We delete abp and bap
	      abs.first--;
	      bas.first--;
	      for (auto at(abp.first); at != abp.second; ++at) {
		(*at)->done--;
		c->time -= (*at)->time;
	      }
	      for (auto at(bap.first); at != bap.second; ++at) {
		(*at)->done--;
		c->time -= (*at)->time;
	      }
	      c->path.erase(abp.first, abp.second);
	      c->path.splice(it, c->path, abp.second, bap.first);
	      c->path.erase(bap.first, bap.second);
	      restart = true;
	      break;
	    } /*else if (is_before<Road*>(bap.second, abp.first, c->path.end())) {
	      // We delete abp and bap
	      abs.first--;
	      bas.first--;
	      for (auto at(abp.first); at != abp.second; ++at) {
		(*at)->done--;
		c->time -= (*at)->time;
	      }
	      for (auto at(bap.first); at != bap.second; ++at) {
		(*at)->done--;
		c->time -= (*at)->time;
	      }
	      c->path.erase(bap.first, bap.second);
	      c->path.splice(other, c->path, bap.second, abp.first);
	      c->path.erase(abp.first, abp.second);
	      restart = true;
	      break;
	      } else {
	      // reinsert abp and bap
	      abs.second.push_back(abp);
	      bas.second.push_back(bap);
	      }*/
	  }
	}
	abs.first++;
	abs.second.push_front({ other, it });
      }
      if (restart) {
	break;
      }
      if (buffer.size() == depth) {
	buffer.pop_back();
      }
      buffer.push_front({ pos, it });
      pos = (*it)->use(pos);
      ++it;
    }
    if (restart) {
      --i;
    }
  }
}

void Routage::kill() {
  long loss[ncars];
  for (int i(0); i < ncars; ++i) {
    loss[i] = 0;
    Car* c = cars[i];
    for (Road* r : c->path) {
      r->done--;
      if (r->done == 0) {
	loss[i] += r->score;
      }
    }
    for (Road* r : c->path) {
      r->done++;
    }
  }
  long imin(0);
  long vmin(loss[0]);
  for (int i(1); i < ncars; ++i) {
    if (loss[i] < vmin) {
      imin = i;
      vmin = loss[i];
    }
  }
  printf("Killing car %ld\n", imin+1);
  Car* c = cars[imin];
  for (Road* r : c->path) {
    r->done--;
  }
  c->time = 0;
  c->path.clear();
}

void Routage::untangle() {
  for (long i(0); i < ncars; ++i) {
    Car *c = cars[i];
    unordered_map<Intersection*, list<Road*>::iterator> visited;
    Intersection *pos(start);
    for (auto it(c->path.begin()), ie(c->path.end()); it != ie; ) {
      // pos -it->
      auto f = visited.find(pos);
      if (f != visited.end()) {
	auto oit = f->second;
	// pos -oit->
	Intersection* npos(pos);
	long d(0), t(0);
	for (auto ft(oit); ft != it; ++ft) {
	  if ((*ft)->done == 1) {
	    d += (*ft)->score;
	  }
	  (*ft)->done--;
	  t += (*ft)->time;
	  visited.erase(npos);
	  npos = (*ft)->use(npos);
	}
	if (t > 5 * d) {
	  printf("[%ld] Expensive cycle found. %ldm in %lds.\n", i, d, t);
	  for (auto ft(oit); ft != it; ++ft) {
	    c->time -= (*ft)->time;
	  }
	  it = c->path.erase(oit, it);
	} else {
	  for (auto ft(oit); ft != it; ++ft) {
	    (*ft)->done++;
	  }
	}
      }
      visited.insert({ pos, it });
      pos = (*it)->use(pos);
      ++it;
    }
  }
}

void Routage::take_deadends() {
  for (long i(0); i < ninters; ++i) {
    if (inters[i]->ends_here.size() > 0) {
      Intersection* pos = inters[i];
      for (Road* r : pos->ends_here) {
	if (r->done == 0 && r->bidir) {
	  Intersection *other = r->use(pos);
	  Road* pre(NULL);
	  for (Road *rue : other->ends_here) {
	    if (rue->done > 0) {
	      pre = rue;
	      break;
	    }
	  }
	  if (pre != NULL) {
	    printf("Small optim: %ld\n", r->score);
	  } else {
	    break;
	  }
	  bool ok = false;
	  for (long j(0); j < ncars; ++j) {
	    Car *c = cars[j];
	    Intersection *pc(start);
	    for (auto it(c->path.begin()), ie(c->path.end()); it != ie; ++it) {
	      if (*it == pre && c->time + 2 * r->time > time) {
		c->want += 2 * r->time;
	      }
	      if (*it == pre && c->time + 2 * r->time <= time) {
		if (pc == other) {
		  c->path.insert(it, r);
		  c->path.insert(it, r);
		} else {
		  ++it;
		  c->path.insert(it, r);
		  c->path.insert(it, r);
		}
		r->done += 2;
		c->time += 2 * r->time;
		ok = true;
		break;
	      }
	      pc = (*it)->use(pc);
	    }
	    if (ok) {
	      break;
	    }
	  }
	}
      }
    }
  }
}

void Routage::do_stuff() {
  long missing(0);
  long d(0);
  long t(0);
  long total(0);
  long dones(0);
  for (int i(0); i < nroads; ++i) {
    dones += roads[i]->done;
    if (!roads[i]->done) {
      printf("Missing road: %ld from (%lf, %lf) to (%lf, %lf) (%ld)\n", i, roads[i]->start->lat, roads[i]->start->lon, roads[i]->end->lat, roads[i]->end->lon, roads[i]->bidir);
      missing++;
      d += roads[i]->score;
      t += roads[i]->time;
    } else {
      total += roads[i]->score;
    }
  }
  int rem(time - cars[0]->time);
  for (int i(0); i < ncars; ++i) {
    if (time - cars[i]->time < rem) {
      rem = time - cars[i]->time;
    }
  }
  printf("Score: %ld.%05ld\n", total, rem);
  printf("Avg redundancy: %ld%\n", dones * 100 / nroads);
  printf("Missing roads: %ld for %ldm (at least %lds)\n", missing, d, t);
  long tot(0);
  for (int i(0); i < ncars; ++i) {
    tot += time - cars[i]->time;
    cars[i]->infos(time);
  }
  printf("Remaining total time: %lds (avg %lds)\n", tot, tot / ncars);
}
