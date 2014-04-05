#include <cmath>
#include "main.h"
#define PI 3.14159265

void print_basic(int& c, int display) {
  int k = floor(sin( static_cast<float>(c) * PI/80.)) ;
  ++c;
  for(int i = 0 ; i < k ; ++i) {
    putchar(' ') ;
  }
  printf("%d", display) ;
}

void sin_printer(int nvehic, Car* cars) {
  int x = 0 ; 
  print_basic(x, nvehic) ;
  for (int i(0); i < nvehic; ++i) {
    Car &c = cars[i];
    print_basic(x, c.path.size());
    for (auto i : c.path) {
      print_basic(x, i);
    }
  }
}
