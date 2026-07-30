#include "core/vec.h"
#include "sg_scheduler.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

SGJScheduler sgjs_new(size_t max_jobs) {
  SGJScheduler sgjs = {0};
  sgjs.max_jobs = max_jobs;
  vec_resize(sgjs.executing, max_jobs);
  return sgjs;
}

SGJob* sgjs_poll(SGJScheduler* sched) {
  // Advance the state of the previously yielded job from STARTING to RUNNING
  if (sched->executing.n > 0) {
    size_t prev_curr = (sched->curr == 0) ? sched->executing.n - 1 : sched->curr - 1;
    if (sched->executing.get[prev_curr].state == SGJS_STARTING) {
      sched->executing.get[prev_curr].state = SGJS_RUNNING;
    }
  }

  while (true) {
    // Populate the sched->executing ring buffer
    while (sched->executing.n < sched->max_jobs && sched->pending.n > 0) {
      vec_push(sched->executing, vec_pop(sched->pending));
      sched->executing.get[sched->executing.n - 1].state = SGJS_STARTING;
    }

    // If theres no more job to execute
    if (sched->executing.n == 0)
      return NULL;

    sched->curr %= sched->executing.n;

    SGJob* job = &sched->executing.get[sched->curr];

    if (job->state != SGJS_STOPPING) {
      // Advance ring buffer
      sched->curr = (sched->curr + 1) % sched->executing.n;
      return job;
    }

    // If the job is stopping and there are other pending tasks
    // then replace the job
    if (sched->pending.n > 0) {
      *job = vec_pop(sched->pending);
      job->state = SGJS_STARTING;
      sched->curr = (sched->curr + 1) % sched->executing.n;
      return job;
    }

    // if there are no more pending jobs but a job is stopping
    // then remove it
    vec_remove_swap(sched->executing, sched->curr);

    if (sched->curr >= sched->executing.n)
      sched->curr = 0;
  }
}

void sgjs_stop(SGJScheduler* sched, SGJob* job) { job->state = SGJS_STOPPING; }
void sgjs_submit(SGJScheduler* sched, SGJob job) { vec_push(sched->pending, job); }

void sgjs_free(SGJScheduler* sched) {
  vec_destroy(sched->executing);
  vec_destroy(sched->pending);
}
