/**
  @file
  Lightweight vector utility
*/

#ifndef VEC_H
#define VEC_H

#include <assert.h> // IWYU pragma: keep
#include <stdint.h> // IWYU pragma: keep
#include <stdlib.h> // IWYU pragma: keep
#include <string.h> // IWYU pragma: keep

/// vec.n => length, vec.m => capacity, vec.get => pointer to the start of the array
#define vec(T)                                                                                     \
  struct {                                                                                         \
    T* get;                                                                                        \
    uint32_t m, n;                                                                                   \
  }

/**
  Grow vector by 2x the current capacity
  @note If the current capacity (vec.m) is 0, it will set it by default to 2
*/
#define vec_grow(vec)                                                                              \
  ((void)(((vec).m = ((vec).m ? (vec).m * 2 : 2)),                                                 \
          (vec).get = realloc((vec).get, (vec).m * sizeof(*(vec).get)),                            \
          0))

/**
  @param n Resize vector to n of elements
  @warning If the resize results in reduction of elements such that vec.m < vec.n, can cause bugs
*/
#define vec_resize(vec, n)                                                                         \
  ((void)((vec).get = realloc((vec).get, n * sizeof(*(vec).get)), (vec).m = n, 0))
#define vec_push(vec, e)                                                                           \
  ((void)((((vec).n == (vec).m) ? vec_grow((vec)) : (void)0), (vec).get[(vec).n++] = (e), 0))
#define vec_pop(vec) (assert((vec).n > 0), (vec).get[--(vec).n])
#define vec_destroy(vec)                                                                           \
  ((void)((vec).get ? (free((vec).get), (vec).get = NULL, (vec).n = 0, (vec).m = 0) : 0))
/// Preserves order and removes an element
#define vec_remove(vec, i)                                                                         \
  ((void)(assert((i) < (vec).n),                                                                   \
          memmove((vec).get + (i), (vec).get + (i) + 1, ((vec).n - (i) - 1) * sizeof(*(vec).get)), \
          --(vec).n,                                                                               \
          0))
/// Remove an element faster without preserving order
#define vec_remove_swap(vec, i)                                                                    \
  ((void)(assert((i) < (vec).n), (vec).get[(i)] = (vec).get[(vec).n - 1], --(vec).n, 0))
/// Resize vector so that vec.m = vec.n (capacity = length)
#define vec_freeze(vec) ((void)((vec).get = realloc((vec).get, (vec).n * sizeof(*(vec).get)), 0))

#endif
