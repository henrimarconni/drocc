#ifndef BITARR_H_
#define BITARR_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t bitword_t;
#define BITS_PER_WORD (sizeof(bitword_t) * 8)

typedef struct {
  bitword_t* words;
  size_t size;
} BitArray;

void bitarr_free(BitArray* ba);
BitArray* bitarr_init(size_t total_bits);
#define bitarr_set(ba, bit)                                                                        \
  ((ba)->words[(bit) / BITS_PER_WORD] |= ((bitword_t)1 << ((bit) % BITS_PER_WORD)))
#define bitarr_get(ba, bit) (((ba)->words[(bit) / BITS_PER_WORD] >> ((bit) % BITS_PER_WORD)) & 1)

#endif
