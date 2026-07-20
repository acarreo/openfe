#include <cassert>
#include <vector>

#include "openfe.h"

using namespace openfe;

int main() {
    if (core_init() != RLC_OK) return 1;
    ep_param_set(B12_P381);

    assert(self_check() == 0);

    {
        auto scheme = OpenFE_createContext(OpenFE_SCHEME_ABDP15, 4);
        ABDP15MPK mpk;
        ABDP15MSK msk;
        scheme->setup(mpk, msk);

        std::vector<long> x = {1, 2, 3, 4};
        std::vector<long> y = {2, -1, 0, 0};
        std::vector<long> y2 = {1, 1, 1, 1};

        ABDP15Ciphertext ct;
        scheme->encrypt(mpk, x, ct);

        ABDP15SecretKey sk, sk2;
        scheme->keygen(msk, y, sk);
        scheme->keygen(msk, y2, sk2);

        assert(scheme->decrypt(mpk, sk, ct) == 0);
        assert(scheme->decrypt(mpk, sk2, ct) == 10);
    }

    {
        auto scheme = OpenFE_createContext(OpenFE_SCHEME_ALS16, 4);
        ALS16MPK mpk;
        ALS16MSK msk;
        scheme->setup(mpk, msk);

        std::vector<long> x = {1, 2, 3, 4};
        std::vector<long> y = {2, -1, 0, 0};
        std::vector<long> y2 = {1, 1, 1, 1};

        ALS16Ciphertext ct;
        scheme->encrypt(mpk, x, ct);

        ALS16SecretKey sk, sk2;
        scheme->keygen(msk, y, sk);
        scheme->keygen(msk, y2, sk2);

        assert(scheme->decrypt(mpk, sk, ct) == 0);
        assert(scheme->decrypt(mpk, sk2, ct) == 10);
    }

    core_clean();
    return 0;
}
