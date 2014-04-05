#include "main.h"

void Rue::set(long id, long a, long b, bool bidir, long cost, long score) {
  this->id = id;
  this->start = a;
  this->end = b;
  this->bidir = bidir;
  this->time = cost;
  this->score = score;
  this->ok = false;
  inters[start].alls.push_back(this);
  if (bidir) {
    inters[end].alls.push_back(this);
  }
}


long Rue::path(long start) {
  if (start == this->start) {
    return this->end;
  } else if (start == this->end && bidir) {
    return this->start;
  } else {
    return -1;
  }
}

std::pair<long, long> Rue::raw(std::set<Rue*> &visited) {
  if (this->ok || visited.find(this) != visited.end()) {
    return std::make_pair(0, this->time);
  } else {
    return std::make_pair(this->score, this->time);
  }
}

std::vector<std::pair<long, long>> Rue::real_gain(long pos, long k, std::set<Rue*> &visited) {
  if (k == 0 || visited.find(this) != visited.end()) {
    return { this->raw(visited) };
  }
  auto nxt = this->path(pos);
  auto p = this->raw(visited);
  std::vector<std::pair<long, long>> out = { p };
  visited.insert(this);
  for (auto r : inters[nxt].alls) {
    auto paths = r->real_gain(nxt, k-1, visited);
    for (auto p2 : paths) {
      out.push_back(std::make_pair(p.first + p2.first, p.second + p2.second));
    }
  }
  visited.erase(this);
  return out;
}

double Rue::gain(long pos) {
  auto stuff = std::set<Rue*>();
  auto paths = this->real_gain(pos, PROFONDEUR, stuff);
  double best = -1.0d;
  for (auto p : paths) {
    auto v = static_cast<double>(p.first) / static_cast<double>(p.second);
    if (best < 0.0d || v > best) {
      best = v;
    }
  }
  return best;
}
