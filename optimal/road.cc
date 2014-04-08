#include "road.hh"
#include "intersection.hh"

Road::Road() {
}

Road::Road(long p_id, Intersection *p_start, Intersection *p_end, bool p_bidir, long p_time, long p_score) :
  id(p_id), start(p_start), end(p_end), bidir(p_bidir), time(p_time), score(p_score), done(0) {
  start->starts_here.insert(this);
  end->ends_here.insert(this);
  if (bidir) {
    start->ends_here.insert(this);
    end->starts_here.insert(this);
  }
}

Intersection* Road::use(Intersection* from) {
  if (from == start) {
    return end;
  } else if (from == end) {
    return start;
  } else {
    return NULL;
  }
}

pair<long, long> Road::raw(unordered_set<Road*> &visited) {
  if (this->done > 0 || visited.find(this) != visited.end()) {
    return make_pair(0, this->time);
  } else {
    return make_pair(this->score, this->time);
  }
}

list<pair<long, long>> Road::real_gain(Intersection* pos, long k, unordered_set<Road*> &visited) {
  if (k == 0 || visited.find(this) != visited.end()) {
    return { this->raw(visited) };
  }
  auto nxt = this->use(pos);
  auto p = this->raw(visited);
  list<pair<long, long>> out = { p };
  visited.insert(this);
  for (auto r : nxt->starts_here) {
    auto paths = r->real_gain(nxt, k-1, visited);
    for (auto p2 : paths) {
      out.push_back(make_pair(p.first + p2.first, p.second + p2.second));
    }
  }
  visited.erase(this);
  return out;
}

double Road::gain(Intersection* pos, long k) {
  auto stuff = unordered_set<Road*>();
  auto paths = this->real_gain(pos, k, stuff);
  double best = -1.0d;
  for (auto p : paths) {
    auto v = static_cast<double>(p.first) / static_cast<double>(p.second);
    if (best < 0.0d || v > best) {
      best = v;
    }
  }
  return best;
}
