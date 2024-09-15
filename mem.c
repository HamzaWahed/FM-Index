#include "fm_index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_LENGTH 40

long long MEMs(char *file, char *rev_file, char *pattern) {
  fm_index *fm = build(file);
  fm_index *rev_fm = build(rev_file);
  long long mem_counts = 0;
  char *tmp = malloc(strlen(pattern));

  for (int i = 0; i <= strlen(pattern);) {
    int j = i + MEM_LENGTH;
    strncpy(tmp, pattern, j);

    while (count(rev_fm, tmp) && j <= strlen(pattern)) {
      j++;
      strncpy(tmp, pattern, j);
    }

    if (j > i + MEM_LENGTH) {
      mem_counts++;
    }

    int k = 0;
    strncpy(tmp, pattern + j + 1, 1);
    while (count(fm, tmp) && (j + 1 - k) >= 0) {
      k++;
      strncpy(tmp, pattern + j + 1 - k, k + 1);
    }

    if (k > 0 && strlen(tmp) > MEM_LENGTH) {
      mem_counts++;
    }

    i = j - k + 1;
  }

  free_fm_index(fm);
  free_fm_index(rev_fm);
  free(tmp);
  return mem_counts;
}

int main(int argc, char **argv) {
  FILE *pattern_file = fopen(argv[3], "r");
  size_t pattern_length = 10000;
  char *pattern = malloc(pattern_length);

  getline(&pattern, &pattern_length, pattern_file);
  printf("mem counts: %lld", MEMs(argv[1], argv[2], pattern));

  free(pattern);
  fclose(pattern_file);
}
