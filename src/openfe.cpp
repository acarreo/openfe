#include "openfe/openfe.h"

#include "bpdpvs.h"

namespace openfe {

int self_check() {
    // Compile-time validation that bpdpvs public types are visible.
    (void)sizeof(G1_VECTOR);
    (void)sizeof(G2_VECTOR);
    return 0;
}

}
