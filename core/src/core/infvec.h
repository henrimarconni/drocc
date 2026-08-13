/**
  @file
  Lightweight infinite vector utility which is backed by demand-paging
*/

#ifndef INFVEC_H_
#define INFVEC_H_

#include "core/vmem_arena.h" // IWYU pragma: keep
#include <assert.h>          // IWYU pragma: keep
#include <stdlib.h>          // IWYU pragma: keep
#include <string.h>          // IWYU pragma: keep

/// vec.n => length, vec.m => capacity, vec.get => pointer to the start of the array
#define infvec(T)                                                                                  \
  struct {                                                                                         \
    T* get;                                                                                        \
    uint32_t m, n;                                                                                 \
  }

#define infvec_init(vec, cap)                                                                      \
  ((vec).get = os_demand_alloc((cap) * sizeof(*(vec).get)), (vec).m = (cap), (vec).n = 0)

#define infvec_push(vec, e) (assert((vec).n < (vec).m), (vec).get[(vec).n++] = (e), 0)

#define infvec_pop(vec) (assert((vec).n > 0), (vec).get[--(vec).n])

#define infvec_destroy(vec)                                                                        \
  (os_demand_free((vec).get, (vec).m * sizeof(*(vec).get)), (vec).get = NULL, (vec).m = (vec).n = 0)

/// Preserves order and removes an element
#define infvec_remove(vec, i)                                                                      \
  (assert((i) < (vec).n),                                                                          \
   memmove((vec).get + (i), (vec).get + (i) + 1, ((vec).n - (i) - 1) * sizeof(*(vec).get)),        \
   --(vec).n,                                                                                      \
   0)

#define infvec_remove_swap(vec, i) (assert((i) < (vec).n), (vec).get[(i)] = (vec).get[--(vec).n], 0)

#endif
