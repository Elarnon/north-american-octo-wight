#include "routage.hh"
#include <iostream>

#include <queue>
#include <unordered_set>

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
  , roads(NULL), nroads(0), cars(NULL), ncars(0), locs(NULL) {
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
  delete[] edges;
  delete[] locs;
  locs = new location[ninters];
  for (long j(0); j < ninters; ++j) {
    locs[j].x = inters[j]->lat;
    locs[j].y = inters[j]->lon;
  }
  edges = new pair<long, long>[nedges];
  long weights[nedges];
  long i(0);
  for (long j(0); j < nroads; ++j) {
    Road* road(roads[j]);
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
  g = Graph(edges, edges + nedges, ninters);
  property_map < Graph, edge_weight_t >::type w = get(edge_weight, g);
  graph_traits < Graph >::edge_iterator e, e_end;
  i = 0;
  for (boost::tie(e, e_end) = boost::edges(g); e != e_end; ++e) {
    w[*e] = weights[i++];
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

list<Road*> Routage::shortest_path(Intersection* from, Intersection* to) {
  vector<Graph::vertex_descriptor> p(num_vertices(g));
  try {
    astar_search(g, from->id,
		 distance_heuristic<Graph, double, location*>(locs, to->id),
    		 predecessor_map(&p[0]).
		 visitor(astar_goal_visitor<Graph::vertex_descriptor>(to->id)));
  } catch (found_goal fg) {
    list<Road*> sp;
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
    return sp;
  }
  return {};
}

void Routage::search(long depth) {
  auto pq = std::priority_queue<Car*, std::vector<Car*>>();
  for (long i(0); i < ncars; ++i) {
    pq.push(cars[i]);
  }
  while (!pq.empty()) {
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
      c->move(best);
      pq.push(c);
    }
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
	if (pruning && 2 * t > 3 * d) {
	  printf("[%ld] Last prune will give %lds and lose %ldm.\n", i, t, d);
	  for (auto ft(it.base()); ft != c->path.end(); ++ft) {
	    (*ft)->done--;
	    c->time -= (*ft)->time;
	  }
	  c->path.erase(it.base(), c->path.end());
	  break;
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

void Routage::prune() {
  for (int i(0); i < ncars; ++i) {
    Car *c(cars[i]);
    pair<Intersection*, Intersection*> p;
    bool optimizing(false);
    Intersection* pos(start);
    long nprunable(0), t(0), d(0), tott(0);
    for (auto it(c->path.begin());
	 it != c->path.end(); ) {
      Intersection* oldpos = pos;
      pos = (*it)->use(pos);
      if ((*it)->done > 1) {
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
	    printf("[%ld] Prunable path of %ldm (%lds). May want to swap.\n", i, d, t);
	  }
	  tott += t;
	  nprunable++;
	  list<Road*> sp(shortest_path(p.first, p.second));
	  c->path.insert(it, sp.begin(), sp.end());
	  for (auto rit(sp.begin()), rend(sp.end()); rit != rend; ++rit) {
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
      // Check if path [at->first, at->second] and [bt->first, bt->second] are coprunables
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

void Routage::elarnon(long depth) {
  for (long i(0); i < ncars; ++i) {
    printf("Car %ld...\n", i);
    bool restart = false;
    Car *c = cars[i];
    Intersection* pos(start);
    list<pair<Intersection*, list<Road*>::iterator>> buffer;
    auto stuff = unordered_map<long, pair<long, list<pair<list<Road*>::iterator, list<Road*>::iterator>>>>();
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
	      printf("spos[%ld] -> pos[%ld]: \n  ", spos->id, pos->id);
	      Intersection* tmp = spos;
	      printf("%ld", spos->id);
	      for (auto at(abp.first); at != abp.second; ++at) {
		printf(" -%ld> %ld", (*at)->id, (tmp = (*at)->use(tmp))->id);
		(*at)->done--;
		c->time -= (*at)->time;
	      }
	      printf("\n");
	      printf("pos[%ld] -> spos[%ld]: \n  ", pos->id, spos->id);
	      tmp = pos;
	      printf("%ld", tmp->id);
	      for (auto at(bap.first); at != bap.second; ++at) {
		printf(" -%ld> %ld", (*at)->id, (tmp = (*at)->use(tmp))->id);
		(*at)->done--;
		c->time -= (*at)->time;
	      }
	      printf("\n");
	      printf("%ld // %ld\n", (*other)->id, (*it)->id);
	      printf("Removing [%ld -> %ld)\n", (*abp.first)->id, (*abp.second)->id);
	      c->path.erase(abp.first, abp.second);
	      printf("Adding [%ld .. %ld) at %ld\n", (*abp.second)->id, (*bap.first)->id, pos->id);
	      c->path.splice(it, c->path, abp.second, bap.first);
	      printf("Removing [%ld .. %ld)\n", (*bap.first)->id, (*bap.second)->id);
	      c->path.erase(bap.first, bap.second);
	      restart = true;
	      break;
	    } else {
	      // reinsert abp and bap
	      abs.second.push_back(abp);
	      bas.second.push_back(bap);
	    }
	  }
	}
	abs.first++;
	abs.second.push_front({ other, it });
      }
      if (restart) {
	break;
      }
      if (buffer.size() == depth) {
	buffer.pop_front();
      }
      buffer.push_back({ pos, it });
      pos = (*it)->use(pos);
      ++it;
    }
    if (restart) {
      --i;
    }
  }
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
	if (t > d) {
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
	    printf("Small optim: %d\n", r->score);
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
  long nim(0);
  long bim(0);
  long total(0);
  long dones(0);
  for (int i(0); i < nroads; ++i) {
    dones += roads[i]->done;
    if (!roads[i]->done) {
      if (roads[i]->end->starts_here.size() <= 1) {
	nim++;
	bim += roads[i]->time;
      } else if (roads[i]->start->starts_here.size() <= 1) {
	nim++;
	bim += roads[i]->time;
      }
      missing++;
      d += roads[i]->score;
      t += roads[i]->time;
    } else {
      total += roads[i]->score;
    }
  }
  printf("Score: %ld\n", total);
  printf("Avg redundancy: %ld%\n", dones * 100 / nroads);
  printf("Missing roads: %ld for %ldm (at least %lds)\n", missing, d, t);
  long tot(0);
  for (int i(0); i < ncars; ++i) {
    tot += time - cars[i]->time;
    cars[i]->infos(time);
  }
  printf("Remaining total time: %lds (avg %lds)\n", tot, tot / ncars);
  printf("Impasses: %ld for %ld\n", nim, bim);
}
