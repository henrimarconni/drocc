#include "chu80/diag.h"

const DiagInfo c80_diaginfos[__c80_diaginfos_len] = {
#define X(_, str, level) {level, str},
    C80_ERRORS(X)
#undef X
};
