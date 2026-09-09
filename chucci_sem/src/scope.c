#include "chucci_sem/scope.h"

ScopeManager new_scope_mgr() {
  ScopeManager mgr = {0};
  mgr.len = 1;
  return mgr;
}

ScopeID new_scope(ScopeManager* mgr) { return mgr->len++; }
