#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1000000
#define MINIBLOCK_SIZE 64
#define MACROBLOCK_SIZE 65536

typedef enum _nucleotides { A = 0, C, G, T } nucleotides;

void setup(char *filename);
int rank1(int i, nucleotides nucleotide);
int rank(nucleotides nucleotide, int i);
int count(char *query);
nucleotides char_to_nucleotide(char c);

int dollarPos;
uint64_t masks[64];

char *bitvectors[4];
uint16_t *miniheaders[4];
uint64_t *macroheaders[4];
uint64_t C_array[4];

int main(int argc, char *argv[]) {

  for (int i = A; i <= T; i++) {
    bitvectors[i] = malloc(SIZE / 8 * sizeof(char));
    miniheaders[i] = malloc((SIZE / MINIBLOCK_SIZE) * sizeof(uint16_t));
    macroheaders[i] =
        malloc(((SIZE + 65535) / MACROBLOCK_SIZE) * sizeof(uint64_t));
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

  return 0;
}

nucleotides char_to_nucleotide(char c) {
  switch (c) {
  case 'A':
    return A;
  case 'C':
    return C;
  case 'G':
    return G;
  case 'T':
    return T;
  default:
    fprintf(stderr, "Invalid nucleotide character: %c\n", c);
    return A;
  }
}

void setup(char *filename) {

  FILE *file = fopen(filename, "r");

  int miniRank[] = {0, 0, 0, 0};
  int currentRank[] = {0, 0, 0, 0};

  for (int i = A; i <= T; i++) {
    memset(bitvectors[i], 0, SIZE / 8);
  }

  for (int i = 0; i < SIZE; i++) {

    if (i % MINIBLOCK_SIZE == 0) {
      for (int j = A; j <= T; j++) {
        miniheaders[j][i / MINIBLOCK_SIZE] = (uint16_t)miniRank[j];
      }
    }

    if (i % MACROBLOCK_SIZE == 0) {
      for (int j = A; j <= T; j++) {
        miniRank[j] = 0;
        macroheaders[j][i / MACROBLOCK_SIZE] = (uint64_t)currentRank[j];
      }
    }

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
    case '$':
      dollarPos = i;
      break;
    };
  }

  fclose(file);

  memset(&masks[63], 255, 8);

  for (int i = 62; i >= 0; i--) {
    memcpy(&masks[i], &masks[i + 1], 8);
    char *charMask = (char *)&masks[i];
    charMask[(i + 1) / 8] = charMask[(i + 1) / 8] & ~(1 << (i + 1) % 8);
  }

  C_array[A] = 0;
  C_array[C] = rank(A, SIZE - 1);
  C_array[G] = rank(A, SIZE - 1) + rank(C, SIZE - 1);
  C_array[T] = rank(A, SIZE - 1) + rank(C, SIZE - 1) + rank(G, SIZE - 1);

  return;
}

int rank(nucleotides nucleotide, int i) {
  if (i < 0) {
    return 0;
  }

  int sum = (int)miniheaders[nucleotide][i / MINIBLOCK_SIZE] +
            (int)macroheaders[nucleotide][i / MACROBLOCK_SIZE];

  uint64_t miniblock = *(uint16_t *)&bitvectors[nucleotide][(i / 64) * 8];

  miniblock = miniblock & masks[i % 64];

  sum += __builtin_popcountll(miniblock);

  return sum;
}

int count(char *query) {
  int m = strlen(query);

  int s = 0;
  int e = SIZE - 1;

  for (int i = m - 1; i >= 0 && s <= e; i--) {
    s = C_array[char_to_nucleotide(query[i])] +
        rank(char_to_nucleotide(query[i]), s - 1) + 1;
    e = C_array[char_to_nucleotide(query[i])] +
        rank(char_to_nucleotide(query[i]), e);
  }

  if (e < s) {
    return 0;
  }

  return e - s + 1;
}
