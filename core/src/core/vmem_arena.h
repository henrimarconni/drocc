/**
  @file
  Provides an arena allocator which uses Demand Paging
*/

#ifndef __VMEM_ARENA_H
#define __VMEM_ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define VMEM_ARENA_MAX_CAP 1024 * 1024

typedef struct VMEMArena {
  uint8_t *data;
  size_t pos;
  size_t cap;
  size_t committed_len;
} VMEMArena;

typedef struct VMEMArenaMark {
  size_t pos;
} VMEMArenaMark;

VMEMArena *vmarena_new(size_t cap);
void *_vmarena_alloc(VMEMArena *arena, size_t size);
void *_vmarena_calloc(VMEMArena *arena, size_t size);
void *_vmarena_realloc(VMEMArena *arena, void *ptr, size_t old_size,
                       size_t new_size);

void vmarena_reset(VMEMArena *arena);
void vmarena_mark_reset(VMEMArena *arena, VMEMArenaMark mark);
VMEMArenaMark vmarena_mark(VMEMArena *arena);
void vmarena_free(VMEMArena *arena);

// #define VMEM_ARENA_DEBUG 1
/**
  If VMEM_ARENA_DEBUG is defined, allocations done using vmarena_** macros
  will print the caller information and the allocation size
*/
#ifdef VMEM_ARENA_DEBUG
#define vmarena_alloc(arena, size)                                             \
  ((printf("Allocation requested by: %s() %s:%d, \npos=%zu size=%zu cap=%zu "      \
           "remaining_cap=%zu\n\n",                                            \
           __func__, __FILE__, __LINE__, (arena)->pos, (size), (arena)->cap,        \
           (arena)->cap - (arena)->pos)),                                      \
   _vmarena_alloc((arena), (size)))

#define vmarena_calloc(arena, size)                                            \
  ((printf(                                                                    \
       "Calloc Allocation requested by: %s() %s:%d, \npos=%zu size=%zu cap=%zu "   \
       "remaining_cap=%zu\n\n",                                                \
       __func__, __FILE__, __LINE__, (arena)->pos, (size), (arena)->cap,            \
       (arena)->cap - (arena)->pos)),                                          \
   _vmarena_calloc((arena), (size)))

#define vmarena_realloc(arena, ptr, old_size, size)                            \
  ((printf("Reallocation requested by: %s() %s:%d, \npos=%zu size=%zu cap=%zu "    \
           "remaining_cap=%zu\n\n",                                            \
           __func__, __FILE__, __LINE__, (arena)->pos, (size), (arena)->cap,        \
           (arena)->cap - (arena)->pos)),                                      \
   _vmarena_realloc((arena), (ptr), (old_size), (size)))

#else
#define vmarena_alloc(arena, size) _vmarena_alloc(arena, size)
#define vmarena_calloc(arena, size) _vmarena_calloc(arena, size)
#define vmarena_realloc(arena, ptr, old_size, size) _vmarena_realloc(arena, ptr, old_size, size) 
#endif

void* os_mmap_file(const char* filepath, size_t* out_size);
void os_unmap_file(void* data, size_t size);

void* os_vm_reserve(size_t size);
void os_vm_commit(void* ptr, size_t size);
void os_vm_free(void* ptr, size_t size);

#endif
