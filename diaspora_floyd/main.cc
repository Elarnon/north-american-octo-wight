#include "main.h"
#include "diaspora.h"

 Intersection *inters;
 long ninters;
 Rue *rues;
 long nrues;
 Car *cars;
 long nvehic;
 long start;
 long total_time;



int main() {
  parse();
  print_diaspora(diaspora()) ;
  return 0 ;
}
 
