#ifndef SG_PROCESS_H
#define SG_PROCESS_H

typedef struct SGProcess SGProcess;

typedef enum {
  SGPROC_CAPTURE_STDOUT = 1 << 0,
  SGPROC_CAPTURE_STDERR = 1 << 1,
} SGProcessFlags;

typedef enum {
  SGPROC_RUNNING,
  SGPROC_EXITED,
  SGPROC_SIGNAL,
  SGPROC_STOPPED,
  SGPROC_TIMEOUT
} SGProcessState;

typedef struct {
  SGProcessState state;
  /// Contains Exit Code for EXITED, Signal Number for SIGNAL/STOPPED, 0 for RUNNING
  int code;
} SGProcessStatus;


#endif
