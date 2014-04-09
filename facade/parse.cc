#include <string>
#include <iostream>
#include <limits>
#include <cstdio>

using namespace std;

int main() {
  int nlines, ncol;
  scanf(" %d %d", &nlines, &ncol);
  bool *arr = new bool[nlines * ncol];
  int tmp;
  fflush(stdin);
  for (int i(0); i < nlines; ++i) {
    for (int j(0); j < ncol; ++j) {
      arr[j + i * ncol] = (getchar() == '#');
    }
    while (getchar() != '\n') ;
  }
  /*
  for (int i(0); i < nlines; ++i) {
    for (int j(0); j < ncol; ++j) {
      if (arr[j + i * ncol]) {
        putchar('#');
      } else {
        putchar('.');
      }
    }
    putchar('\n');
    }*/
}
