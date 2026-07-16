#include <cassert>
#include <vector>

#include "openfe.h"

using namespace openfe;

int main() {
    if (core_init() != RLC_OK) return 1;
    if (ep_param_set(B12_P381) != RLC_OK) {
        core_clean();
        return 1;
    }

    assert(self_check() == 0);

    auto scheme = OpenFE_createContext(OpenFE_SCHEME_ABDP15, 4);
    ABDP15MPK mpk;
    ABDP15MSK msk;
    scheme->setup(mpk, msk);

    ZP z0(0u), z1(1u), z2(2u), z3(3u), z4(4u);
    std::vector<ZP> x = {z1, z2, z3, z4};
    std::vector<ZP> y = {z2, z0 - z1, z0, z0};
    std::vector<ZP> y2 = {z1, z1, z1, z1};

    ABDP15Ciphertext ct;
    scheme->encrypt(mpk, x, ct);

    ABDP15SecretKey sk, sk2;
    scheme->keygen(msk, y, sk);
    scheme->keygen(msk, y2, sk2);

    assert(scheme->decrypt(mpk, sk, ct) == 0);
    assert(scheme->decrypt(mpk, sk2, ct) == 10);

    core_clean();
    return 0;
}
