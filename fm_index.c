#include "fm_index.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1000000
#define MINIBLOCK_SIZE 64
#define MACROBLOCK_SIZE 65536

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
    exit(EXIT_FAILURE);
  }
}

void free_fm_index(fm_index *fm) {
  for (int i = A; i <= T; i++) {
    free(fm->bitvectors[i]);
    free(fm->miniheaders[i]);
    free(fm->macroheaders[i]);
  }

  free(fm);
  return;
}

fm_index *build(char *filename) {
  fm_index *fm = malloc(sizeof(fm_index));

  for (int i = A; i <= T; i++) {
    fm->bitvectors[i] = malloc(SIZE / 8 * sizeof(char));
    fm->miniheaders[i] = malloc((SIZE / MINIBLOCK_SIZE) * sizeof(uint16_t));
    fm->macroheaders[i] =
        malloc(((SIZE + 65535) / MACROBLOCK_SIZE) * sizeof(uint64_t));
  }

  FILE *file = fopen(filename, "r");

  int miniRank[] = {0, 0, 0, 0};
  int currentRank[] = {0, 0, 0, 0};

  for (int i = A; i <= T; i++) {
    memset(fm->bitvectors[i], 0, SIZE / 8);
  }

  for (int i = 0; i < SIZE; i++) {

    if (i % MINIBLOCK_SIZE == 0) {
      for (int j = A; j <= T; j++) {
        fm->miniheaders[j][i / MINIBLOCK_SIZE] = (uint16_t)miniRank[j];
      }
    }

    if (i % MACROBLOCK_SIZE == 0) {
      for (int j = A; j <= T; j++) {
        miniRank[j] = 0;
        fm->miniheaders[j][i / MINIBLOCK_SIZE] = 0;
        fm->macroheaders[j][i / MACROBLOCK_SIZE] = (uint64_t)currentRank[j];
      }
    }

    char c = (char)fgetc(file);

    switch (c) {
    case 'A':
      fm->bitvectors[A][i / 8] = fm->bitvectors[A][i / 8] | (1 << i % 8);
      miniRank[A]++;
      currentRank[A]++;
      break;
    case 'C':
      fm->bitvectors[C][i / 8] = fm->bitvectors[C][i / 8] | (1 << i % 8);
      miniRank[C]++;
      currentRank[C]++;
      break;
    case 'G':
      fm->bitvectors[G][i / 8] = fm->bitvectors[G][i / 8] | (1 << i % 8);
      miniRank[G]++;
      currentRank[G]++;
      break;
    case 'T':
      fm->bitvectors[T][i / 8] = fm->bitvectors[T][i / 8] | (1 << i % 8);
      miniRank[T]++;
      currentRank[T]++;
      break;
    case '$':
      break;
    };
  }

  fclose(file);

  memset(&fm->masks[63], 255, 8);

  for (int i = 62; i >= 0; i--) {
    memcpy(&fm->masks[i], &fm->masks[i + 1], 8);
    char *charMask = (char *)&fm->masks[i];
    charMask[(i + 1) / 8] = charMask[(i + 1) / 8] & ~(1 << (i + 1) % 8);
  }

  fm->C_array[A] = 0;
  fm->C_array[C] = rank(fm, A, SIZE - 1);
  fm->C_array[G] = rank(fm, A, SIZE - 1) + rank(fm, C, SIZE - 1);
  fm->C_array[T] =
      rank(fm, A, SIZE - 1) + rank(fm, C, SIZE - 1) + rank(fm, G, SIZE - 1);

  return fm;
}

int rank(fm_index *fm, nucleotides nucleotide, int i) {
  if (i < 0) {
    return 0;
  }

  int macroblock_index = i / MACROBLOCK_SIZE;
  int miniblock_index = i / MINIBLOCK_SIZE;

  int sum = (int)fm->macroheaders[nucleotide][macroblock_index] +
            (int)fm->miniheaders[nucleotide][miniblock_index];

  uint64_t miniblock = *(uint64_t *)&fm->bitvectors[nucleotide][(i / 64) * 8];

  miniblock = miniblock & fm->masks[i % 64];

  sum += __builtin_popcountll(miniblock);

  return sum;
}

int count(fm_index *fm, char *query) {
  int m = strlen(query);

  int s = 0;
  int e = SIZE - 1;

  for (int i = m - 1; i >= 0 && s <= e; i--) {
    s = fm->C_array[char_to_nucleotide(query[i])] +
        rank(fm, char_to_nucleotide(query[i]), s - 1) + 1;
    e = fm->C_array[char_to_nucleotide(query[i])] +
        rank(fm, char_to_nucleotide(query[i]), e);
  }

  if (e < s) {
    return 0;
  }

  return e - s + 1;
}
