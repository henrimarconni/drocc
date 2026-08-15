#include "core/vec.h"
#include "sgrun/sg_scheduler.h"
#include "sgrun/sg_sleep.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SGJS_FPS 240

SGJScheduler sgjs_new(uint32_t max_jobs) {
  SGJScheduler sgjs = {0};
  sgjs.max_jobs = max_jobs;
  vec_resize(sgjs.executing, max_jobs);
  return sgjs;
}

void sgjs_await(SGJScheduler* sched) {
  while (sched->executing.n > 0 || sched->pending.n > 0) {
    // fill the executing queue up to max_jobs
    while (sched->executing.n < sched->max_jobs && sched->pending.n > 0) {
      SGFuture new_task = vec_pop(sched->pending);

      if (new_task.start(new_task.ctx))
        vec_push(sched->executing, new_task);
      else
        printf("Failed to start job\n");
    }

    // poll all executing jobs
    for (size_t i = 0; i < sched->executing.n; i++) {
      SGFuture* future = &sched->executing.get[i];
      bool running = future->poll(future->ctx);

      if (!running) {
        // remove and swap the last element into this spot
        vec_remove_swap(sched->executing, i);

        // go again for this future, because last element of the queue was swapped with the current
        // (stopped) one and we want to poll it
        i--;
      }
    }

    sleep_ms(1000 / SGJS_FPS);
  }
}

void sgjs_free(SGJScheduler* sched) {
  vec_destroy(sched->executing);
  vec_destroy(sched->pending);
}

void sgjs_submit(SGJScheduler* sched, void* ctx, PollFutFn poll, StartFutFn start) {
  SGFuture future = {0};
  future.poll = poll;
  future.start = start;
  future.ctx = ctx;

  if (sched->executing.n < sched->max_jobs) {
    if (!start(ctx))
      return;
    vec_push(sched->executing, future);
  } else
    vec_push(sched->pending, future);
}
