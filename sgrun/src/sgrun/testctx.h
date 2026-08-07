#ifndef TESTCTX_H_
#define TESTCTX_H_


#include "core/stringbuilder.h"
#include "loader.h"
#include "process.h"
#include <stddef.h>


/// Context for each job
typedef struct {
  struct SGTest* test;
  size_t id;
  SGProcess* proc;
  SGRunnerOptions* rs;
  size_t* passed;
  size_t* failed;
  StringBuilder err;
  StringBuilder out;
  int fmt_width;
  size_t time;
} SGTestCtx;

SGTestCtx new_test_ctx(size_t id, struct SGTest* test, SGRunnerOptions* rs, size_t* passed,
                              size_t* failed, int fmt_width);
bool start_new_test_ctx(void* ctx);
void print_test_ctx(SGProcessStatus* status, SGTestCtx* ctx);
bool poll_test_ctx(void* ctx);

#endif
