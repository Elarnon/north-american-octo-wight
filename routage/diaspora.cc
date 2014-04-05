#include "diaspora.h"
#include <cmath>

struct diaspora_data {
public:
  diaspora_data() : path(), cost(0.f) {}
  diaspora_data(double x) : path(), cost(x) {}

  std::list<Rue*> path ;
  double cost ;
};

diaspora_data** to_matrix(Routage* routage) {
  diaspora_data** matrix = new (diaspora_data*)[routage->ninters] ;
  for(size_t i = 0 ; i < routage->ninters ; ++i) {
    matrix[i] = new diaspora_data[routage->ninters] ;
    for(size_t j = 0 ; j < routage->ninters ; ++j) {
      matrix[i][j] = diaspora_data(i == j ? 0. : std::numeric_limits<double>::infinity());
    }
  }
  
  for(long i = 0 ; i < routage->nrues ; ++i) {
    Rue* r = routage->rues[i] ;

    matrix[r->start][r->end].cost = r.time ;
    matrix[r->start][r->end].path.push_back(r) ;
    
    if(r.bidir) {
          matrix[r->end][r->start].cost = r.time ;
	  matrix[r->end][r->start].path.push_back(r) ;
    }
  }

  return matrix ;
}

void floyd_warshall(diaspora_data** matrix, long n) {
  for(long k = 0 ; k < n ; ++k) {
    for(long i = 0 ; i < n ; ++i) {
      for(long j = 0 ; j < n ; ++j) {
	double c = matrix[i][k].cost + matrix[k][j].cost ;
	if(matrix[i][j] > c) {
	  matrix[i][j].cost = c ;
	  std::list<Rue*> p = matrix[i][k].paths ;
	  std::list<Rue*> q = matrix[k][j].paths ;
	  std::list<Rue*> res = std::list(p) ;
	  res.insert(res.end(), q.begin(), q.end()) ;
	  matrix[i][j].path = res ;
	}
      }
    }
  }
}

std::list<Rue*>* diaspora_aux(Intersection* londre, Routage* routage) {
  double r2 = sqrt(2.) ;
  double x[] = { 1., r2, 0., -r2, -1., -r2,  0.,  r2} ;
  double y[] = { 0., r2, 1.,  r2,  0., -r2, -1., -r2} ;
  double kappa = 3. ;
  double centerLat = 0. ;
  double centerLon = 0. ;
  Intersection** res = { 
    londre, londre, 
    londre, londre,
    londre, londre,
    londre, londre
  }
  float* gain = { 0., 0., 0., 0., 0., 0., 0., 0. } ;
  
  for(long but_index = 0 ; i < routage->ninters ; ++but_index) {
    Intersection* but = routage->inters[but_index] ;
    for(long i = 0 ; i < 8 ; ++i) {
      double gain0 = x[i] * (but->lat - centerLat) + y[i] * (but->lon - centerLon) ;
      double gain = 
	copysign(pow(gain0, kappa), gain0) / matrix[londre->id][but->id].cost ;
      if (gain[i] < gain) {
	gain[i] = gain ;
	res[i] = but ;
      }
    }
  }

  std::list<Rue*>* paths[8] ;
  for(long i = 0 ; i < 8 ; ++i) {
    paths[i] = matrix[londre->id][res[i]->id] ;
  }

  return paths ;
}


std::list<Rue*>*  diaspora(Intersection* londre, Routage* routage) {
  diaspora_data** matrix = to_matrix(routage) ;
  floyd_warshall(matrix) ;
  return diaspora_aux(londre, routage) ;
}
