#ifndef SCOPE_H_
#define SCOPE_H_

#include <stdint.h>

typedef struct {
  uint32_t len;
} ScopeManager;

typedef uint32_t ScopeID;

ScopeManager new_scope_mgr();
ScopeID new_scope(ScopeManager* mgr);

#endif
