#include "openfe/contextksw08.h"
#include <stdexcept>
#include <typeinfo>

namespace openfe {


// gp=(g,0,0), gq=(0,g,0), gr=(0,0,g) : les 3 vecteurs de base des sous-espaces orthogonaux
OpenFEContextKSW08::OpenFEContextKSW08(size_t vector_len) : ell(vector_len) {
    ord = group.getGroupOrder();

    G1 g1_gen; g1_gen.setGenerator();
    G2 g2_gen; g2_gen.setGenerator();
    G1 g1_inf;
    G2 g2_inf;

    g1_gp = G1_VECTOR(3);
    g1_gp.insertElement(g1_gen, 0);
    g1_gp.insertElement(g1_inf, 1);
    g1_gp.insertElement(g1_inf, 2);

    g2_gp = G2_VECTOR(3);
    g2_gp.insertElement(g2_gen, 0);
    g2_gp.insertElement(g2_inf, 1);
    g2_gp.insertElement(g2_inf, 2);

    g1_gq = G1_VECTOR(3);
    g1_gq.insertElement(g1_inf, 0);
    g1_gq.insertElement(g1_gen, 1);
    g1_gq.insertElement(g1_inf, 2);

    g2_gq = G2_VECTOR(3);
    g2_gq.insertElement(g2_inf, 0);
    g2_gq.insertElement(g2_gen, 1);
    g2_gq.insertElement(g2_inf, 2);

    g1_gr = G1_VECTOR(3);
    g1_gr.insertElement(g1_inf, 0);
    g1_gr.insertElement(g1_inf, 1);
    g1_gr.insertElement(g1_gen, 2);

    g2_gr = G2_VECTOR(3);
    g2_gr.insertElement(g2_inf, 0);
    g2_gr.insertElement(g2_inf, 1);
    g2_gr.insertElement(g2_gen, 2);
}

// mpk/msk partagent les mêmes a1,a2 par coordonnée, mpk masqué par du bruit R
void OpenFEContextKSW08::setup(OpenFEMPK &gen_mpk, OpenFEMSK &gen_msk) {
    auto &mpk = dynamic_cast<KSW08MPK&>(gen_mpk);
    auto &msk = dynamic_cast<KSW08MSK&>(gen_msk);
    ZP r0; r0.setRandom(ord);
    G1_VECTOR R0 = g1_gr * r0;

    mpk.Q = g1_gq + R0;
    msk.g_q = g2_gq;

    mpk.H1.resize(ell);
    mpk.H2.resize(ell);
    msk.h1.resize(ell);
    msk.h2.resize(ell);

    for (size_t i = 0; i < ell; ++i) {
        ZP r1_prime, r2_prime, a1, a2;
        r1_prime.setRandom(ord);
        r2_prime.setRandom(ord);
        a1.setRandom(ord);
        a2.setRandom(ord);

        G1_VECTOR R1 = g1_gr * r1_prime;
        G1_VECTOR R2 = g1_gr * r2_prime;

        mpk.H1[i] = (g1_gp * a1) + R1;
        mpk.H2[i] = (g1_gp * a2) + R2;

        msk.h1[i] = g2_gp * a1;
        msk.h2[i] = g2_gp * a2;
    }
}

// chiffre x, alpha/beta le masquent, s relie toutes les coordonnées
void OpenFEContextKSW08::encrypt(const OpenFEMPK &gen_mpk, const std::vector<ZP> &x, OpenFECiphertext &gen_ct) {
    auto &mpk = dynamic_cast<const KSW08MPK&>(gen_mpk);
    auto &ct = dynamic_cast<KSW08Ciphertext&>(gen_ct);
    if (x.size() != ell) throw std::runtime_error("Encryption vector size mismatch");

    ZP s, alpha, beta;
    s.setRandom(ord);
    alpha.setRandom(ord);
    beta.setRandom(ord);

    ct.C0 = g1_gp * s;
    ct.C1.resize(ell);
    ct.C2.resize(ell);

    for (size_t i = 0; i < ell; ++i) {
        ZP r3_prime, r4_prime;
        r3_prime.setRandom(ord);
        r4_prime.setRandom(ord);

        G1_VECTOR R3 = g1_gr * r3_prime;
        G1_VECTOR R4 = g1_gr * r4_prime;

        ZP ax = alpha * x[i];
        ZP bx = beta * x[i];

        ct.C1[i] = (mpk.H1[i] * s) + (mpk.Q * ax) + R3;
        ct.C2[i] = (mpk.H2[i] * s) + (mpk.Q * bx) + R4;
    }
}

// r1[i]/r2[i] réutilisés dans K et K1/K2 pour s'annuler au déchiffrement
void OpenFEContextKSW08::keygen(const OpenFEMSK &gen_msk, const std::vector<ZP> &y, OpenFESecretKey &gen_sk) {
    auto &msk = dynamic_cast<const KSW08MSK&>(gen_msk);
    auto &sk = dynamic_cast<KSW08SecretKey&>(gen_sk);
    if (y.size() != ell) throw std::runtime_error("KeyGen vector size mismatch");

    std::vector<ZP> r1(ell), r2(ell);
    for (size_t i = 0; i < ell; ++i) {
        r1[i].setRandom(ord);
        r2[i].setRandom(ord);
    }

    ZP r5_prime; r5_prime.setRandom(ord);
    G2_VECTOR R5 = g2_gr * r5_prime;

    ZP f1, f2;
    f1.setRandom(ord);
    f2.setRandom(ord);

    ZP q6_prime; q6_prime.setRandom(ord);
    G2_VECTOR Q6 = g2_gq * q6_prime;

    G2 g2_inf;
    G2_VECTOR sum_h(3);
    sum_h.insertElement(g2_inf, 0);
    sum_h.insertElement(g2_inf, 1);
    sum_h.insertElement(g2_inf, 2);

    for (size_t i = 0; i < ell; ++i) {
        ZP neg_r1 = -r1[i];
        ZP neg_r2 = -r2[i];
        sum_h = sum_h + (msk.h1[i] * neg_r1) + (msk.h2[i] * neg_r2);
    }

    sk.K = R5 + Q6 + sum_h;

    sk.K1.resize(ell);
    sk.K2.resize(ell);

    for (size_t i = 0; i < ell; ++i) {
        ZP fy1 = f1 * y[i];
        ZP fy2 = f2 * y[i];
        sk.K1[i] = (g2_gp * r1[i]) + (g2_gq * fy1);
        sk.K2[i] = (g2_gp * r2[i]) + (g2_gq * fy2);
    }
}

// couple C0/C1/C2 avec K/K1/K2, le résultat se réduit à e(g,g)^{(a f1+b f2)<x,y>}
bool OpenFEContextKSW08::decrypt(const OpenFESecretKey &gen_sk, const OpenFECiphertext &gen_ct) {
    auto &sk = dynamic_cast<const KSW08SecretKey&>(gen_sk);
    auto &ct = dynamic_cast<const KSW08Ciphertext&>(gen_ct);
    if (ct.C1.size() != ell || sk.K1.size() != ell) throw std::runtime_error("Decryption component size mismatch");

    GT D = innerProduct(ct.C0, sk.K);
    for (size_t i = 0; i < ell; ++i) {
        D = D * innerProduct(ct.C1[i], sk.K1[i]) * innerProduct(ct.C2[i], sk.K2[i]);
    }

    return D.isIdentity();
}

}
