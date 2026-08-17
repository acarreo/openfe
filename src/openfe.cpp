#include "openfe.h"
#include "bpdpvs.h"

namespace openfe {

int self_check() {
    (void)sizeof(G1_VECTOR);
    (void)sizeof(G2_VECTOR);
    return 0;
}

}
