#include "openfe.h"
#include "bpdpvs.h"

namespace openfe {

// vérifie juste que les types bpdpvs sont complets à la compilation
int self_check() {
    (void)sizeof(G1_VECTOR);
    (void)sizeof(G2_VECTOR);
    return 0;
}

}
