#include "core/bitarr.h"

BitArray* bitarr_init(size_t total_bits) {
  BitArray* ba = (BitArray*)malloc(sizeof(BitArray));
  ba->size = (total_bits + BITS_PER_WORD - 1) / BITS_PER_WORD;
  ba->words = (bitword_t*)calloc(ba->size, sizeof(bitword_t));
  if (!ba->words) {
    free(ba);
    return NULL;
  }
  return ba;
}

void bitarr_free(BitArray* ba) {
  if (ba) {
    free(ba->words);
    free(ba);
  }
}
