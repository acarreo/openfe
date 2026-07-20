#include "openfe/contextals16.h"
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

OpenFEContextALS16::OpenFEContextALS16(size_t vector_len, long bound) : ell(vector_len), bound(bound) {
    ord = group.getGroupOrder();
}

void OpenFEContextALS16::setup(OpenFEMPK &gen_mpk, OpenFEMSK &gen_msk) {
    auto &mpk = dynamic_cast<ALS16MPK &>(gen_mpk);
    auto &msk = dynamic_cast<ALS16MSK &>(gen_msk);

    mpk.g.setGenerator();
    mpk.h.setRandom();
    mpk.h_i.resize(ell);
    msk.s.resize(ell);
    msk.t.resize(ell);

    for (size_t i = 0; i < ell; ++i) {
        msk.s[i].setRandom(ord);
        msk.t[i].setRandom(ord);
        mpk.h_i[i] = mpk.g * msk.s[i] + mpk.h * msk.t[i];
    }
}

void OpenFEContextALS16::encrypt(const OpenFEMPK &gen_mpk, const std::vector<long> &x, OpenFECiphertext &gen_ct) {
    auto &mpk = dynamic_cast<const ALS16MPK &>(gen_mpk);
    auto &ct = dynamic_cast<ALS16Ciphertext &>(gen_ct);
    if (x.size() != ell) throw std::runtime_error("Encryption vector size mismatch");

    ZP r; r.setRandom(ord);
    ct.C = mpk.g * r;
    ct.D = mpk.h * r;
    ct.E.resize(ell);
    for (size_t i = 0; i < ell; ++i) {
        ct.E[i] = mpk.g * longToZP(x[i], ord) + mpk.h_i[i] * r;
    }
}

void OpenFEContextALS16::keygen(const OpenFEMSK &gen_msk, const std::vector<long> &y, OpenFESecretKey &gen_sk) {
    auto &msk = dynamic_cast<const ALS16MSK &>(gen_msk);
    auto &sk = dynamic_cast<ALS16SecretKey &>(gen_sk);
    if (y.size() != ell) throw std::runtime_error("KeyGen vector size mismatch");

    sk.y.resize(ell);
    sk.s_y = ZP(0u);
    sk.t_y = ZP(0u);
    for (size_t i = 0; i < ell; ++i) {
        sk.y[i] = longToZP(y[i], ord);
        sk.s_y = sk.s_y + sk.y[i] * msk.s[i];
        sk.t_y = sk.t_y + sk.y[i] * msk.t[i];
    }
}

long OpenFEContextALS16::decrypt(const OpenFEMPK &gen_mpk, const OpenFESecretKey &gen_sk, const OpenFECiphertext &gen_ct) {
    auto &mpk = dynamic_cast<const ALS16MPK &>(gen_mpk);
    auto &sk = dynamic_cast<const ALS16SecretKey &>(gen_sk);
    auto &ct = dynamic_cast<const ALS16Ciphertext &>(gen_ct);
    if (ct.E.size() != ell || sk.y.size() != ell) throw std::runtime_error("Decryption component size mismatch");

    G1 Ey;
    for (size_t i = 0; i < ell; ++i) {
        Ey = Ey + ct.E[i] * sk.y[i];
    }
    Ey = Ey + ct.C * (-sk.s_y) + ct.D * (-sk.t_y);

    return discreteLogBSGS(mpk.g, Ey, bound);
}

}
