#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup(char *filename);
int rank(char c, int i);
int count(char *query);

int *rankArray;
int dollarPos;

int main(int argc, char *argv[]) {

  rankArray = (int *)malloc(1000000 * sizeof(int));

  setup(argv[1]);

  char query[101];

  while (scanf("%100s", query) == 1) {
    printf("%i occurrences of %s.\n", count(query), query);
  }

  return 0;
}

void setup(char *filename) {

  FILE *file = fopen(filename, "r");

  int currentRank = 0;

  for (int i = 0; i < 1000000; i++) {

    char c = (char)fgetc(file);

    if (c == 'B') {
      currentRank++;
    }

    rankArray[i] = currentRank;

    if (c == '$') {
      dollarPos = i;
    }
  }

  fclose(file);
  return;
}

int rank(char c, int i) {

  if (i < 0) {
    return (0);
  }

  if (c == 'A') {
    if (i >= dollarPos) {
      return ((i + 1) - rankArray[i] - 1);
    } else {
      return ((i + 1) - rankArray[i]);
    }
  } else {
    return (rankArray[i]);
  }
}

int count(char *query) {

  int m = strlen(query);

  int s = 0;
  int e = 999999;

  for (int i = m - 1; i >= 0 && s <= e; i--) {

    if (query[i] == 'A') {
      s = rank('A', s - 1) + 1;
      e = rank('A', e);
    } else {
      s = rank('A', 999999) + rank('B', s - 1) + 1;
      e = rank('A', 999999) + rank('B', e);
    }
  }

  return (e - s + 1);
}
