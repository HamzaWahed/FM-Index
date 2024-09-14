#ifndef __FM_INDEX
#include <stdint.h>

typedef enum _nucleotides { A = 0, C, G, T } nucleotides;

typedef struct _fm_index {
  char *bitvectors[4];
  uint16_t *miniheaders[4];
  uint64_t *macroheaders[4];
  uint64_t C_array[4];
  uint64_t masks[64];
} fm_index;

fm_index *setup(char *filename);
int rank(fm_index *fm, nucleotides nucleotide, int i);
int count(fm_index *fm, char *query);
nucleotides char_to_nucleotide(char c);
void free_fm_index(fm_index *fm);

#endif
