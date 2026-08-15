#ifndef SG_SCHEDULER_H
#define SG_SCHEDULER_H

#include "core/vec.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef bool(*PollFutFn)(void*);
typedef bool(*StartFutFn)(void*);

typedef struct {
  void* ctx;
  PollFutFn poll;
  StartFutFn start;
} SGFuture;

typedef struct {
  vec(SGFuture) pending;
  vec(SGFuture) executing;
  uint32_t max_jobs;
  /// id of current job
  uint32_t curr;
} SGJScheduler;


SGJScheduler sgjs_new(uint32_t max_jobs);

/// Await all the jobs added, blocks till all jobs are completed
void sgjs_await(SGJScheduler* sched);

/// Pushes the job to pending queue
void sgjs_submit(SGJScheduler* sched, void* ctx, PollFutFn poll, StartFutFn start);


/// Free the scheduler
void sgjs_free(SGJScheduler* sched);

#endif
