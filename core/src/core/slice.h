/**
  @file
  Lightweight slice utility
*/

#ifndef SLICE_H
#define SLICE_H

#include <assert.h> // IWYU pragma: keep
#include <stdint.h> // IWYU pragma: keep
#include <stdlib.h> // IWYU pragma: keep
#include <string.h> // IWYU pragma: keep

/// slice.n => length, slice.get => pointer to the start of the array
#define slice(T)                                                                                   \
  struct {                                                                                         \
    T* get;                                                                                        \
    uint32_t n;                                                                                    \
  }

#define vec_slice(vec) {.get = (vec).get, .n = (vec).n}

#endif
