#include "openfe/contextabdp15.h"
#include "openfe/dlog.h"
#include <stdexcept>

namespace openfe {

namespace {

ZP longToZP(long v, const ZP &ord) {
    unsigned long mag = v < 0 ? static_cast<unsigned long>(-v) : static_cast<unsigned long>(v);

    ZP base(1u);
    base.setOrder(ord.m_ZP);
    for (int i = 0; i < 32; ++i) base = base + base;

    ZP acc(static_cast<uint32_t>(mag >> 32));
    acc.setOrder(ord.m_ZP);
    acc = acc * base + ZP(static_cast<uint32_t>(mag & 0xffffffffu));
    return v < 0 ? -acc : acc;
}

}

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

void OpenFEContextABDP15::encrypt(const OpenFEMPK &gen_mpk, const std::vector<long> &x, OpenFECiphertext &gen_ct) {
    auto &mpk = dynamic_cast<const ABDP15MPK &>(gen_mpk);
    auto &ct = dynamic_cast<ABDP15Ciphertext &>(gen_ct);
    if (x.size() != ell) throw std::runtime_error("Encryption vector size mismatch");

    ZP r; r.setRandom(ord);
    ct.ct0 = mpk.g * r;
    ct.ct.resize(ell);
    for (size_t i = 0; i < ell; ++i) {
        ct.ct[i] = mpk.h[i] * r + mpk.g * longToZP(x[i], ord);
    }
}

void OpenFEContextABDP15::keygen(const OpenFEMSK &gen_msk, const std::vector<long> &y, OpenFESecretKey &gen_sk) {
    auto &msk = dynamic_cast<const ABDP15MSK &>(gen_msk);
    auto &sk = dynamic_cast<ABDP15SecretKey &>(gen_sk);
    if (y.size() != ell) throw std::runtime_error("KeyGen vector size mismatch");

    sk.y.resize(ell);
    ZP acc(0u);
    for (size_t i = 0; i < ell; ++i) {
        sk.y[i] = longToZP(y[i], ord);
        acc = acc + sk.y[i] * msk.s[i];
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
    D = D + ct.ct0 * (-sk.sk_y);

    return discreteLogBSGS(mpk.g, D, bound);
}

}
