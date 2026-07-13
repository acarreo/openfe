#include <cassert>
#include <vector>

#include "openfe.h"

using namespace openfe;

int main() {
    if (core_init() != RLC_OK) return 1;
    if (pc_param_set_any() != RLC_OK) {
        core_clean();
        return 1;
    }

    assert(self_check() == 0);

    auto scheme = OpenFE_createContext(OpenFE_SCHEME_KSW08, 4);
    KSW08MPK mpk;
    KSW08MSK msk;
    scheme->setup(mpk, msk);

    ZP zero(0u), one(1u), two(2u), three(3u), four(4u);
    ZP neg_one = zero - one;

    std::vector<ZP> x = {one, two, three, four};
    std::vector<ZP> y = {two, neg_one, zero, zero};
    std::vector<ZP> y2 = {one, one, one, one};

    KSW08Ciphertext ct;
    scheme->encrypt(mpk, x, ct);

    KSW08SecretKey sk, sk2;
    scheme->keygen(msk, y, sk);
    scheme->keygen(msk, y2, sk2);

    assert(scheme->decrypt(sk, ct) == true);
    assert(scheme->decrypt(sk2, ct) == false);



    core_clean();
    return 0;
}
