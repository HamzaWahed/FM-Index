#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _nucleotides { A, C, G, T } nucleotides;

void setup(char *filename);
int rank1(int i, nucleotides nucleotide);
int rank(nucleotides nucleotide, int i);
int count(char *query);

int dollarPos;
uint64_t masks[64];
char **bitvectors;
int **miniheaders;
int **macroheaders;

int main(int argc, char *argv[]) {

  bitvectors = malloc(4 * sizeof(char *));
  miniheaders = malloc(4 * sizeof(int *));
  macroheaders = malloc(4 * sizeof(int *));

  for (int i = A; i <= T; i++) {
    bitvectors[i] = malloc(1000000 / 8);
    miniheaders[i] = malloc(1000000 / 32);
    macroheaders[i] = malloc((1065535 / 65536) * 8);
  }

  setup(argv[1]);

  char query[101];

  while (scanf("%100s", query) == 1) {
    printf("%i occurrences of %s.\n", count(query), query);
  }

  for (int i = A; i <= T; i++) {
    free(bitvectors[i]);
    free(miniheaders[i]);
    free(macroheaders[i]);
  }

  free(bitvectors);
  free(miniheaders);
  free(macroheaders);

  return 0;
}

void setup(char *filename) {

  FILE *file = fopen(filename, "r");

  int miniRank[] = {0, 0, 0, 0};
  int currentRank[] = {0, 0, 0, 0};

  for (int i = A; i <= T; i++) {
    memset(bitvectors[i], 0, 1000000 / 8);
  }

  for (int i = 0; i < 1000000; i++) {

    char c = (char)fgetc(file);

    switch (c) {
    case 'A':
      bitvectors[A][i / 8] = bitvectors[A][i / 8] | (1 << i % 8);
      miniRank[A]++;
      currentRank[A]++;
      break;
    case 'C':
      bitvectors[C][i / 8] = bitvectors[C][i / 8] | (1 << i % 8);
      miniRank[C]++;
      currentRank[C]++;
      break;
    case 'G':
      bitvectors[G][i / 8] = bitvectors[G][i / 8] | (1 << i % 8);
      miniRank[G]++;
      currentRank[G]++;
      break;
    case 'T':
      bitvectors[T][i / 8] = bitvectors[T][i / 8] | (1 << i % 8);
      miniRank[T]++;
      currentRank[T]++;
      break;
    };

    if (i % 64 == 0) {
      for (int j = A; j <= T; j++) {
        miniheaders[j][i / 64] = (uint16_t)miniRank[j];
      }
    }

    if (i % 65536 == 0) {
      for (int j = A; j <= T; j++) {
        miniRank[j] = 0;
        macroheaders[j][i / 65536] = (uint64_t)currentRank[j];
      }
    }

    if (c == '$') {
      dollarPos = i;
    }
  }

  fclose(file);

  memset(&masks[63], 255, 8);

  for (int i = 62; i >= 0; i--) {
    memcpy(&masks[i], &masks[i + 1], 8);
    char *charMask = (char *)&masks[i];
    charMask[(i + 1) / 8] = charMask[(i + 1) / 8] & ~(1 << (i + 1) % 8);
  }

  return;
}

int rank1(int i, nucleotides nucleotide) {

  int sum = (int)miniheaders[nucleotide][i / 64] +
            (int)macroheaders[nucleotide][i / 65536];

  uint64_t miniblock = bitvectors[nucleotide][(i / 64) * 8];

  miniblock = miniblock & masks[i % 64];

  sum += __builtin_popcountll(miniblock);

  return sum;
}

int rank(nucleotides nucleotide, int i) {

  if (i < 0) {
    return 0;
  }

  int rank1i = rank1(i, nucleotide);

  if (i >= dollarPos) {
    return (i + 1) - rank1i - 1;
  }

  return ((i + 1) - rank1i);
}

/**
 * Backward Search Algorithm
 */
int count(char *query) {

  int m = strlen(query);

  int s = 0;
  int e = 999999;

  for (int i = m - 1; i >= 0 && s <= e; i--) {
    s = rank(A, s - 1) + 1;
    e = rank(A, e);
  }

  if (e < s)
    return 0;

  return e - s + 1;
}
