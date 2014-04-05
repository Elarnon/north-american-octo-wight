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

std::pair<long, long> Rue::raw(std::vector<Rue*> &visited) {
  if (this->ok || std::find(visited.begin(), visited.end(), this) != visited.end()) {
    return std::make_pair(0, this->time);
  } else {
    return std::make_pair(this->score, this->time);
  }
}

std::vector<std::pair<long, long>> Rue::real_gain(long pos, long k, std::vector<Rue*> &visited) {
  if (k == 0 || std::find(visited.begin(), visited.end(), this) != visited.end()) {
    return { this->raw(visited) };
  }
  auto nxt = this->path(pos);
  auto p = this->raw(visited);
  std::vector<std::pair<long, long>> out = { p };
  visited.push_back(this);
  for (auto r : inters[nxt].alls) {
    auto paths = r->real_gain(nxt, k-1, visited);
    for (auto p : paths) {
      out.push_back(std::make_pair(p.first + p.first, p.second + p.second));
    }
  }
  visited.pop_back();
  return out;
}

double Rue::gain(long pos) {
  auto stuff = std::vector<Rue*>();
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
