#ifndef SG_CAPTURE_H
#define SG_CAPTURE_H

#include "sg_api.h"
#include <stdio.h>

void capture_begin(SGCapture* cap, FILE* stream);
char* capture_end(SGCapture* cap);

#endif
