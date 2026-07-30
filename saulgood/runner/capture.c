#define SG_RUNNER_DEV
#include "sg_api.h"
#include <assert.h>
#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#define sg_dup _dup
#define sg_dup2 _dup2
#define sg_fileno _fileno
#define sg_close _close
#else
#include <unistd.h>
#define sg_dup dup
#define sg_dup2 dup2
#define sg_fileno fileno
#define sg_close close
#endif

void capture_begin(SGCapture* cap, FILE* stream) {
  fflush(stream);

  cap->stream = stream;
  cap->saved_fd = sg_dup(sg_fileno(stream));
  assert(cap->saved_fd != -1);

  cap->tmp = tmpfile();
  assert(cap->tmp);

  assert(sg_dup2(sg_fileno(cap->tmp), sg_fileno(stream)) != -1);
}

void capture_discard(SGCapture* cap) {
  fflush(cap->stream);
  fflush(cap->tmp);

  dup2(cap->saved_fd, fileno(cap->stream));
  close(cap->saved_fd);

  fclose(cap->tmp);

  cap->saved_fd = -1;
  cap->tmp = NULL;
}

char* capture_end(SGCapture* cap) {
  fflush(cap->stream);

  fseek(cap->tmp, 0, SEEK_END);
  long len = ftell(cap->tmp);
  rewind(cap->tmp);

  char* buf = malloc((size_t)len + 1);
  assert(buf);

  fread(buf, 1, (size_t)len, cap->tmp);
  buf[len] = '\0';

  fflush(cap->tmp);

  assert(sg_dup2(cap->saved_fd, sg_fileno(cap->stream)) != -1);
  sg_close(cap->saved_fd);

  fclose(cap->tmp);

  return buf;
}
