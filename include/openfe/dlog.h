#ifndef OPENFE_DLOG_H
#define OPENFE_DLOG_H

#include "bpdpvs.h"

namespace openfe {

long discreteLogBSGS(const G1 &g, const G1 &h, long bound);
long discreteLogBSGSGT(const GT &g, const GT &h, long bound);

}

#endif
