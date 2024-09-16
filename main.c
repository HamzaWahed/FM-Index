#include "fm_index.h"
#include <stdio.h>

int main(int argc, char **argv) {
  fm_index *fm = build(argv[1]);
  char query[101];

  while (scanf("%100s", query) == 1) {
    printf("%i occurrences of %s.\n", count(fm, query), query);
  }

  return 0;
}
