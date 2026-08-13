/**
  @file
  Lightweight infinite vector utility which is backed by demand-paging
*/

#ifndef INFVEC_H_
#define INFVEC_H_

// Number of elements to bump the internal commit tracker by at once.
#define COMMIT_LEN 128

// OS Page alignment helper (4096 bytes) to prevent mprotect/VirtualAlloc crashes
#define INFVEC_ALIGN_UP(x) (((x) + 4095) & ~4095)

#include "core/vmem_arena.h" // IWYU pragma: keep
#include <assert.h>          // IWYU pragma: keep
#include <stdint.h>          // For uint32_t
#include <stdlib.h>          // IWYU pragma: keep
#include <string.h>          // IWYU pragma: keep

/// vec.n => length, vec.m => capacity, vec.get => pointer to the start of the array, vec.committed
/// => internal length to commit more memory
#define infvec(T)                                                                                  \
  struct {                                                                                         \
    T* get;                                                                                        \
    uint32_t m, n, committed;                                                                      \
  }

#define infvec_init(vec, cap)                                                                      \
  ((vec).get = os_vm_reserve((cap) * sizeof(*(vec).get)),                                          \
   (vec).m = (cap),                                                                                \
   (vec).n = 0,                                                                                    \
   (vec).committed = 0)

#define infvec_push(vec, e)                                                                        \
  (assert((vec).n < (vec).m),                                                                      \
   ((vec).n >= (vec).committed)                                                                    \
       ? (((INFVEC_ALIGN_UP(((vec).committed + COMMIT_LEN) * sizeof(*(vec).get)) >                 \
            INFVEC_ALIGN_UP((vec).committed * sizeof(*(vec).get)))                                 \
               ? (os_vm_commit(                                                                    \
                      (char*)(vec).get + INFVEC_ALIGN_UP((vec).committed * sizeof(*(vec).get)),    \
                      INFVEC_ALIGN_UP(((vec).committed + COMMIT_LEN) * sizeof(*(vec).get)) -       \
                          INFVEC_ALIGN_UP((vec).committed * sizeof(*(vec).get))),                  \
                  0)                                                                               \
               : 0),                                                                               \
          (vec).committed += COMMIT_LEN)                                                           \
       : 0,                                                                                        \
   (vec).get[(vec).n++] = (e),                                                                     \
   0)

#define infvec_pop(vec) (assert((vec).n > 0), (vec).get[--(vec).n])

#define infvec_destroy(vec)                                                                        \
  (os_vm_free((vec).get, (vec).m * sizeof(*(vec).get)),                                            \
   (vec).get = NULL,                                                                               \
   (vec).m = (vec).n = (vec).committed = 0)

/// Preserves order and removes an element
#define infvec_remove(vec, i)                                                                      \
  (assert((i) < (vec).n),                                                                          \
   memmove((vec).get + (i), (vec).get + (i) + 1, ((vec).n - (i) - 1) * sizeof(*(vec).get)),        \
   --(vec).n,                                                                                      \
   0)

#define infvec_remove_swap(vec, i) (assert((i) < (vec).n), (vec).get[(i)] = (vec).get[--(vec).n], 0)

#endif
