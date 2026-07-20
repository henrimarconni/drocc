#ifndef VEC_H
#define VEC_H

#include <stdlib.h> // IWYU pragma: keep

// Usage: vec.get[i] = element, etc.
#define vec(T) struct { T* get; size_t m, n; }
#define vec_grow(vec) ( ((vec).m = ((vec).m ? (vec).m * 2 : 2) ) , (vec).get = realloc((vec).get, (vec).m * sizeof(*(vec).get)))
#define vec_resize(vec, n) ( (vec).get = realloc((vec).get, n * sizeof(*(vec).get)), (vec).m = n )
#define vec_push(vec, e) ( ( ((vec).n == (vec).m) ? vec_grow((vec)) : 0), (vec).get[(vec).n++] = (e) )
#define vec_pop(vec) (assert((vec).n > 0), (vec).get[--(vec).n])
#define vec_destroy(vec) free((vec).get)
#define vec_freeze(vec) ((vec).get = realloc((vec).get, (vec).n * sizeof(*(vec).get)))

#endif
