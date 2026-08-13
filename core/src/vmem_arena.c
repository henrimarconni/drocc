#include "core/vmem_arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/mman.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#error "Unsupported platform"
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define ALIGN_UP(n, a) (((n) + (a) - 1) & ~((a) - 1))
#define DEFAULT_ALIGNMENT 8

void* os_demand_alloc(size_t size) {
  void* data = NULL;
#if defined(__unix__) || defined(__APPLE__)
  data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(data != MAP_FAILED);
#elif defined(_WIN32)
  data = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  assert(data);
#endif
  return data;
}

void os_demand_free(void* data, size_t size) {
  if (!data)
    return;
#if defined(__unix__) || defined(__APPLE__)
  assert(munmap(data, size) != -1);
#elif defined(_WIN32)
  assert(VirtualFree(data, 0, MEM_RELEASE) != 0);
#endif
}

VMEMArena* vmarena_new(size_t cap) {
  VMEMArena* arena = malloc(sizeof(VMEMArena));
  arena->pos = 0;
  arena->cap = cap;
  arena->data = os_demand_alloc(cap);
  return arena;
}

void* _vmarena_alloc(VMEMArena* arena, size_t size) {
  arena->pos = ALIGN_UP(arena->pos, DEFAULT_ALIGNMENT);
  assert(arena->cap - arena->pos >= size && arena->data);
  arena->pos += size;
  return arena->data + arena->pos - size;
}

void* _vmarena_calloc(VMEMArena* arena, size_t size) {
  void* ptr = _vmarena_alloc(arena, size);
  memset(ptr, 0, size);
  return ptr;
}

void* _vmarena_realloc(VMEMArena* arena, void* ptr, size_t old_size, size_t new_size) {
  if (old_size >= new_size)
    return ptr;
  else if (arena->data + arena->pos == ptr + old_size) {
    arena->pos += new_size - old_size;
    return ptr;
  } else {
    void* new_ptr = _vmarena_alloc(arena, new_size);
    memcpy(new_ptr, ptr, old_size);
    return new_ptr;
  }
}

void vmarena_reset(VMEMArena* arena) { arena->pos = 0; }

void vmarena_mark_reset(VMEMArena* arena, VMEMArenaMark mark) { arena->pos = mark.pos; }

VMEMArenaMark vmarena_mark(VMEMArena* arena) { return (VMEMArenaMark){.pos = arena->pos}; }

void vmarena_free(VMEMArena* arena) {
  os_demand_free(arena->data, arena->cap);
  free(arena);
}
