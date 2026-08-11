#ifndef SLICE_H_
#define SLICE_H_

#include <stdint.h>

typedef struct {
  void* data;
  uint32_t len;
} MemSlice;

#define slice_new(_data, _len)                                                                     \
  (MemSlice) { .data = (_data), .len = (_len) }

#define slice_consume(slice, T)                                                                    \
  (assert((slice).len >= sizeof(T)),                                                               \
   (slice).len -= sizeof(T),                                                                       \
   (slice).data = (char*)(slice).data + sizeof(T),                                                 \
   (T*)((char*)(slice).data - sizeof(T)))

#define slice_get(slice, T, offset)                                                                \
  (assert((slice).len >= (offset) + sizeof(T)), (T*)((char*)(slice).data + (offset)))

#endif
