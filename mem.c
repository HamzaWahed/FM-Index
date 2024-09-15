#include "fm_index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_LENGTH 40
#define TEXT_LENGTH 1000000

uint64_t MEMs(char *file, char *rev_file, char *pattern) {
  fm_index *fm = build(file);
  fm_index *rev_fm = build(rev_file);
  uint64_t mem_counts = 0, pattern_index, current_index = 0;
  char *tmp = malloc(MEM_LENGTH);

  while (current_index <= strlen(pattern)) {
    int s = 0;
    int e = TEXT_LENGTH - 1;
    strncpy(tmp, pattern, current_index + MEM_LENGTH);

    if (count(fm, tmp)) {
      for (pattern_index = current_index + MEM_LENGTH + 1;
           pattern_index < strlen(pattern) && s <= e; pattern_index++) {
        s = rev_fm->C_array[char_to_nucleotide(pattern[pattern_index])] +
            rank(rev_fm, char_to_nucleotide(pattern[pattern_index]), s - 1) + 1;
        e = rev_fm->C_array[char_to_nucleotide(pattern[pattern_index])] +
            rank(rev_fm, char_to_nucleotide(pattern[pattern_index]), e);
      }

      mem_counts++;
    }

    if (pattern_index == strlen(pattern)) {
      break;
    }

    s = 0;
    e = TEXT_LENGTH - 1;
    for (current_index = pattern_index + 1; current_index >= 0 && s <= e;
         current_index--) {
      s = fm->C_array[char_to_nucleotide(pattern[current_index])] +
          rank(fm, char_to_nucleotide(pattern[current_index]), s - 1) + 1;
      e = fm->C_array[char_to_nucleotide(pattern[current_index])] +
          rank(fm, char_to_nucleotide(pattern[current_index]), e);
    }
  }

  free(tmp);
  free_fm_index(fm);
  free_fm_index(rev_fm);
  return mem_counts;
}

int main(int argc, char **argv) {
  FILE *pattern_file = fopen(argv[3], "r");
  size_t max_pattern_length = 10000;
  char *pattern = malloc(max_pattern_length);

  getline(&pattern, &max_pattern_length, pattern_file);
  printf("mem counts: %lld", MEMs(argv[1], argv[2], pattern));

  free(pattern);
  fclose(pattern_file);
}
