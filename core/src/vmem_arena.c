#include "core/vmem_arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__unix__) || defined(__APPLE__)
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#error "Unsupported platform"
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define COMMIT_SIZE 4096

void* os_mmap_file(const char* filepath, size_t* out_size) {
#if defined(__unix__) || defined(__APPLE__)
  int fd = open(filepath, O_RDONLY);
  assert(fd != -1);

  struct stat sb;
  int res = fstat(fd, &sb);
  assert(res != -1);
  *out_size = sb.st_size;

  if (*out_size == 0) {
    close(fd);
    return NULL;
  }

  // Map the file into virtual memory
  void* data = mmap(NULL, *out_size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(data != MAP_FAILED);

  close(fd);
  return data;

#elif defined(_WIN32)
  HANDLE hFile = CreateFileA(
      filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  assert(hFile != INVALID_HANDLE_VALUE);

  LARGE_INTEGER size;
  GetFileSizeEx(hFile, &size);
  *out_size = size.QuadPart;

  if (*out_size == 0) {
    CloseHandle(hFile);
    return NULL;
  }

  HANDLE hMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  assert(hMap != NULL);

  void* data = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
  assert(data != NULL);

  CloseHandle(hMap);
  CloseHandle(hFile);
  return data;
#endif
}

void os_unmap_file(void* data, size_t size) {
  if (!data)
    return;
#if defined(__unix__) || defined(__APPLE__)
  munmap(data, size);
#elif defined(_WIN32)
  UnmapViewOfFile(data);
#endif
}

#define ALIGN_UP(n, a) (((n) + (a) - 1) & ~((a) - 1))
#define DEFAULT_ALIGNMENT 8

void* os_vm_reserve(size_t size) {
#if defined(_WIN32)
  return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
#else
  return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
}

void os_vm_commit(void* ptr, size_t size) {
#if defined(_WIN32)
  VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
#else
  mprotect(ptr, size, PROT_READ | PROT_WRITE);
#endif
}

void os_vm_free(void* ptr, size_t size) {
#if defined(_WIN32)
  VirtualFree(ptr, 0, MEM_RELEASE);
#else
  munmap(ptr, size);
#endif
}

VMEMArena* vmarena_new(size_t cap) {
  VMEMArena* arena = malloc(sizeof(VMEMArena));
  arena->pos = 0;
  arena->cap = cap;
  arena->data = os_vm_reserve(cap);
  arena->committed_len = 0;
  return arena;
}

void* _vmarena_alloc(VMEMArena* arena, size_t size) {
  arena->pos = ALIGN_UP(arena->pos, DEFAULT_ALIGNMENT);

  while (arena->pos + size > arena->committed_len) {
    void* commit_ptr = (char*)arena->data + arena->committed_len;
    os_vm_commit(commit_ptr, COMMIT_SIZE);
    arena->committed_len += COMMIT_SIZE;
  }

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
  os_vm_free(arena->data, arena->cap);
  free(arena);
}
