#ifndef SG_CAPTURE_H
#define SG_CAPTURE_H

#define SG_RUNNER_DEV
#include "sg_api.h"
#include <stdio.h>

void capture_begin(SGCapture* cap, FILE* stream);
void capture_discard(SGCapture* cap);
char* capture_end(SGCapture* cap);

#endif
