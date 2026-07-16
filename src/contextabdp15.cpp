#include "openfe/contextabdp15.h"
#include "openfe/dlog.h"
#include <stdexcept>

namespace openfe {

OpenFEContextABDP15::OpenFEContextABDP15(size_t vector_len, long bound) : ell(vector_len), bound(bound) {
    ord = group.getGroupOrder();
}

void OpenFEContextABDP15::setup(OpenFEMPK &gen_mpk, OpenFEMSK &gen_msk) {
    auto &mpk = dynamic_cast<ABDP15MPK &>(gen_mpk);
    auto &msk = dynamic_cast<ABDP15MSK &>(gen_msk);

    mpk.g.setGenerator();
    mpk.h.resize(ell);
    msk.s.resize(ell);

    for (size_t i = 0; i < ell; ++i) {
        msk.s[i].setRandom(ord);
        mpk.h[i] = mpk.g * msk.s[i];
    }
}

void OpenFEContextABDP15::encrypt(const OpenFEMPK &gen_mpk, const std::vector<ZP> &x, OpenFECiphertext &gen_ct) {
    auto &mpk = dynamic_cast<const ABDP15MPK &>(gen_mpk);
    auto &ct = dynamic_cast<ABDP15Ciphertext &>(gen_ct);
    if (x.size() != ell) throw std::runtime_error("Encryption vector size mismatch");

    ZP r; r.setRandom(ord);
    ct.ct0 = mpk.g * r;
    ct.ct.resize(ell);
    for (size_t i = 0; i < ell; ++i) {
        ct.ct[i] = mpk.h[i] * r + mpk.g * x[i];
    }
}

void OpenFEContextABDP15::keygen(const OpenFEMSK &gen_msk, const std::vector<ZP> &y, OpenFESecretKey &gen_sk) {
    auto &msk = dynamic_cast<const ABDP15MSK &>(gen_msk);
    auto &sk = dynamic_cast<ABDP15SecretKey &>(gen_sk);
    if (y.size() != ell) throw std::runtime_error("KeyGen vector size mismatch");

    sk.y = y;
    ZP acc(0u);
    for (size_t i = 0; i < ell; ++i) {
        acc = acc + y[i] * msk.s[i];
    }
    sk.sk_y = acc;
}

long OpenFEContextABDP15::decrypt(const OpenFEMPK &gen_mpk, const OpenFESecretKey &gen_sk, const OpenFECiphertext &gen_ct) {
    auto &mpk = dynamic_cast<const ABDP15MPK &>(gen_mpk);
    auto &sk = dynamic_cast<const ABDP15SecretKey &>(gen_sk);
    auto &ct = dynamic_cast<const ABDP15Ciphertext &>(gen_ct);
    if (ct.ct.size() != ell || sk.y.size() != ell) throw std::runtime_error("Decryption component size mismatch");

    G1 D;
    for (size_t i = 0; i < ell; ++i) {
        D = D + ct.ct[i] * sk.y[i];
    }
    D = D + ct.ct0 * (ZP(0u) - sk.sk_y);

    return discreteLogBSGS(mpk.g, D, bound);
}

}
