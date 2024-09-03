#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  char *T = (char *)malloc(1000000);

  FILE *file = fopen(argv[1], "r");

  for (int i = 0; i < 1000000; i++) {
    T[i] = (char)fgetc(file);
  }

  fclose(file);

  char query[101];

  while (scanf("%100s", query) == 1) {

    int m = strlen(query);
    int count = 0;

    for (int i = 0; i < 1000000; i++) {
      for (int j = 0; j < m && i + j < 1000000; j++) {
        if (T[i + j] != query[j]) {
          break;
        }

        if (j == m - 1) {
          count++;
        }
      }
    }

    printf("%i occurrences of %s.\n", count, query);
  }

  return 0;
}
