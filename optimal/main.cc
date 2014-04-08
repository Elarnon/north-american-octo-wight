#include "routage.hh"
#include <cstdio>

int main(int argc, char* argv[]) {
  Routage routage;
  routage.parse(stdin);
  if (argc > 1) {
    FILE* f(fopen(argv[1], "r"));
    routage.parse_sol(f);
    fclose(f);
  } else {
    exit(17); // TODO: routage.search();
  }
  routage.make_graph();
  printf("Before: \n");
  routage.do_stuff();
  routage.correct();
  printf("After: \n");
  routage.do_stuff();
  if (argc > 2) {
    FILE* f(fopen(argv[2], "w"));
    routage.print(f);
    fclose(f);
  }
  return 0;
}
