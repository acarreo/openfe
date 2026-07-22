#include "openfe/contextklmmrw16.h"
#include "openfe/dlog.h"
#include <stdexcept>

namespace openfe {

namespace {

using Mat = std::vector<std::vector<ZP>>;

ZP zc(uint32_t v, const ZP &ord) { ZP r(v); r.setOrder(ord.m_ZP); return r; }

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

bool invert(Mat A, size_t ell, const ZP &ord, Mat &inv, ZP &det) {
    inv.assign(ell, std::vector<ZP>(ell));
    for (size_t i = 0; i < ell; ++i)
        for (size_t j = 0; j < ell; ++j) inv[i][j] = zc(i == j ? 1u : 0u, ord);
    det = zc(1u, ord);

    for (size_t col = 0; col < ell; ++col) {
        size_t piv = col;
        while (piv < ell && A[piv][col] == ZP(0u)) ++piv;
        if (piv == ell) return false;
        if (piv != col) { std::swap(A[piv], A[col]); std::swap(inv[piv], inv[col]); det = -det; }

        ZP invPiv = zc(1u, ord) / A[col][col];
        det = det * A[col][col];
        for (size_t j = 0; j < ell; ++j) { A[col][j] = A[col][j] * invPiv; inv[col][j] = inv[col][j] * invPiv; }

        for (size_t row = 0; row < ell; ++row) {
            if (row == col) continue;
            ZP factor = A[row][col];
            if (factor == ZP(0u)) continue;
            for (size_t j = 0; j < ell; ++j) {
                A[row][j] = A[row][j] - factor * A[col][j];
                inv[row][j] = inv[row][j] - factor * inv[col][j];
            }
        }
    }
    return true;
}

Mat randomInvertible(size_t ell, ZP &ord, Mat &inv, ZP &det) {
    while (true) {
        Mat B(ell, std::vector<ZP>(ell));
        for (size_t i = 0; i < ell; ++i)
            for (size_t j = 0; j < ell; ++j) B[i][j].setRandom(ord);
        if (invert(B, ell, ord, inv, det)) return B;
    }
}

std::vector<ZP> vecMul(const std::vector<ZP> &v, const Mat &M, size_t ell, const ZP &ord) {
    std::vector<ZP> r(ell);
    for (size_t j = 0; j < ell; ++j) {
        ZP acc = zc(0u, ord);
        for (size_t i = 0; i < ell; ++i) acc = acc + v[i] * M[i][j];
        r[j] = acc;
    }
    return r;
}

}

OpenFEContextKLMMRW16::OpenFEContextKLMMRW16(size_t vector_len, long bound) : ell(vector_len), bound(bound) {
    ord = group.getGroupOrder();
}

void OpenFEContextKLMMRW16::setup(OpenFEMPK &gen_mpk, OpenFEMSK &gen_msk) {
    auto &mpk = dynamic_cast<KLMMRW16MPK &>(gen_mpk);
    auto &msk = dynamic_cast<KLMMRW16MSK &>(gen_msk);

    msk.g1.setGenerator();
    mpk.g2.setGenerator();

    Mat Binv;
    ZP det;
    msk.B = randomInvertible(ell, ord, Binv, det);
    msk.detB = det;

    mpk.Bstar.assign(ell, std::vector<ZP>(ell));
    for (size_t i = 0; i < ell; ++i)
        for (size_t j = 0; j < ell; ++j) mpk.Bstar[i][j] = det * Binv[j][i];
}

void OpenFEContextKLMMRW16::encrypt(const OpenFEMPK &gen_mpk, const std::vector<long> &x, OpenFECiphertext &gen_ct) {
    auto &mpk = dynamic_cast<const KLMMRW16MPK &>(gen_mpk);
    auto &ct = dynamic_cast<KLMMRW16Ciphertext &>(gen_ct);
    if (x.size() != ell) throw std::runtime_error("Encryption vector size mismatch");

    ZP beta; beta.setRandom(ord);
    ct.C1 = mpk.g2 * beta;

    std::vector<ZP> xv(ell);
    for (size_t i = 0; i < ell; ++i) xv[i] = longToZP(x[i], ord);
    std::vector<ZP> xBstar = vecMul(xv, mpk.Bstar, ell, ord);

    ct.C2.resize(ell);
    for (size_t j = 0; j < ell; ++j) ct.C2[j] = mpk.g2 * (beta * xBstar[j]);
}

void OpenFEContextKLMMRW16::keygen(const OpenFEMSK &gen_msk, const std::vector<long> &y, OpenFESecretKey &gen_sk) {
    auto &msk = dynamic_cast<const KLMMRW16MSK &>(gen_msk);
    auto &sk = dynamic_cast<KLMMRW16SecretKey &>(gen_sk);
    if (y.size() != ell) throw std::runtime_error("KeyGen vector size mismatch");

    ZP alpha; alpha.setRandom(ord);
    sk.K1 = msk.g1 * (alpha * msk.detB);

    std::vector<ZP> yv(ell);
    for (size_t i = 0; i < ell; ++i) yv[i] = longToZP(y[i], ord);
    std::vector<ZP> yB = vecMul(yv, msk.B, ell, ord);

    sk.K2.resize(ell);
    for (size_t j = 0; j < ell; ++j) sk.K2[j] = msk.g1 * (alpha * yB[j]);
}

long OpenFEContextKLMMRW16::decrypt(const OpenFEMPK &, const OpenFESecretKey &gen_sk, const OpenFECiphertext &gen_ct) {
    auto &sk = dynamic_cast<const KLMMRW16SecretKey &>(gen_sk);
    auto &ct = dynamic_cast<const KLMMRW16Ciphertext &>(gen_ct);
    if (sk.K2.size() != ell || ct.C2.size() != ell) throw std::runtime_error("Decryption component size mismatch");

    GT D1 = pairing(sk.K1, ct.C1);
    GT D2; D2.setIdentity();
    for (size_t i = 0; i < ell; ++i) D2 = D2 * pairing(sk.K2[i], ct.C2[i]);

    return discreteLogBSGSGT(D1, D2, bound);
}

}
