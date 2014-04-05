#include "diaspora.h"
#include <cmath>

struct diaspora_data {
public:
  diaspora_data() : path(), cost(0.f) {}
  diaspora_data(double x) : path(), cost(x) {}

  std::list<Rue* > path ;
  double cost ;
};

diaspora_data** to_matrix() {
  diaspora_data** matrix = new diaspora_data*[ninters] ;
  for(size_t i = 0 ; i < ninters ; ++i) {
    matrix[i] = new diaspora_data[ninters] ;
    for(size_t j = 0 ; j < ninters ; ++j) {
      matrix[i][j] = diaspora_data(i == j ? 0. : std::numeric_limits<double>::infinity());
    }
  }
  
  for(long i = 0 ; i < nrues ; ++i) {
    Rue* r = rues + i ;

    matrix[r->start][r->end].cost = r->time ;
    matrix[r->start][r->end].path.push_back(r) ;
    
    if(r->bidir) {
          matrix[r->end][r->start].cost = r->time ;
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
	if(matrix[i][j].cost > c) {
	  matrix[i][j].cost = c ;
	  std::list<Rue*> p = matrix[i][k].path ;
	  std::list<Rue*> q = matrix[k][j].path ;
	  std::list<Rue*> res = std::list<Rue*>(p) ;
	  res.insert(res.end(), q.begin(), q.end()) ;
	  matrix[i][j].path = res ;
	}
      }
    }
  }
}

std::list<Rue*>* diaspora_aux(diaspora_data** matrix) {
  double r2 = sqrt(2.) ;
  double x[] = { 1., r2, 0., -r2, -1., -r2,  0.,  r2} ;
  double y[] = { 0., r2, 1.,  r2,  0., -r2, -1., -r2} ;
  double kappa = 3. ;
  double centerLat = 48.859800 ;
  double centerLon = 2.340953 ;
  long buts[8] ;
  double gain_buts[8] ;
  for(int i = 0 ; i < 8 ; ++i) {
    buts[i] = start ;
    gain_buts[i] = 0. ;
  }
  
  for(long but_index = 0 ; but_index < ninters ; ++but_index) {
    Intersection& but = inters[but_index] ;
    for(int i = 0 ; i < 8 ; ++i) {
      double gain0 = x[i] * (but.lat - centerLat) + y[i] * (but.lon - centerLon) ;
      double gain = 
	copysign(pow(gain0, kappa), gain0) / matrix[start][but.id].cost ;
      if (gain_buts[i] < gain) {
	gain_buts[i] = gain ;
	buts[i] = but.id ;
      }
    }
  }

  std::list<Rue* >* paths = new std::list<Rue* >[8];
  for(long i = 0 ; i < 8 ; ++i) {
    paths[i] = matrix[start][buts[i]].path ;
  }

  return paths ;
}


std::list<Rue*>*  diaspora() {
  diaspora_data** matrix = to_matrix() ;
  floyd_warshall(matrix, ninters) ;
  return diaspora_aux(matrix) ;
}

void print_diaspora(std::list<Rue*>* paths) {
  for(int i = 0 ; i < 8 ; ++i) {
    printf("%d\n", paths[i].size()) ;
    
    std::list<Rue*>::const_iterator iterator;
    for (iterator = paths[i].begin(); iterator != paths[i].end(); ++iterator) {
      printf("%ld\n", (*iterator)->id) ;
    }
  }
}

std::list<long>* read_diaspora() {
  std::list<long>* paths = new std::list<long>[8] ;
  for(int i = 0 ; i < 8 ; ++i) {
    int path_size = 0 ;
    scanf(" %d", &path_size) ;
    for(int j = 0 ; j < path_size ; ++j) {
      long rue_index ;
      scanf(" %ld", rue_index) ;
      paths[i].push_back(rue_index) ;
    }
  }
  return paths ;
}
