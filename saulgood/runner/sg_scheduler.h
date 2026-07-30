#ifndef SG_SCHEDULER_H
#define SG_SCHEDULER_H

#include "core/vec.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum {
  SGJS_RUNNING,
  SGJS_STOPPING,
  SGJS_STARTING,
} SGJobState;

typedef struct {
  void* data;
  SGJobState state;
} SGJob;

typedef struct {
  vec(SGJob) pending;
  vec(SGJob) executing;
  size_t max_jobs;
  /// id of current job
  size_t curr;
} SGJScheduler;


SGJScheduler sgjs_new(size_t max_jobs);
/**
  Poll through the jobs, and populate the  SGJSEvent* event
  @return pointer to a job if there are jobs left, else return NULL
*/
SGJob* sgjs_poll(SGJScheduler* sched);

/**
  Marks the job as SGJS_STOPPING and then, pops it the next time it is polled,
  replacing it with another job from the pending queue
*/
void sgjs_stop(SGJScheduler* sched, SGJob* job);

/// Pushes the job to pending queue
void sgjs_submit(SGJScheduler* sched, SGJob job);

/// Free the scheduler
void sgjs_free(SGJScheduler* sched);

#endif
