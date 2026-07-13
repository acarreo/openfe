#ifndef OPENFE_CONTEXTKSW08_H
#define OPENFE_CONTEXTKSW08_H

#include "openfe/context.h"
#include "bpdpvs.h"
#include <vector>

namespace openfe {

// clé publique : Q + H1/H2 par coordonnée
class KSW08MPK : public OpenFEMPK {
public:
    G1_VECTOR Q;
    std::vector<G1_VECTOR> H1;
    std::vector<G1_VECTOR> H2;
};

// secret maître : g_q + h1/h2 par coordonnée
class KSW08MSK : public OpenFEMSK {
public:
    G2_VECTOR g_q;
    std::vector<G2_VECTOR> h1;
    std::vector<G2_VECTOR> h2;
};

// chiffré d'un vecteur x
class KSW08Ciphertext : public OpenFECiphertext {
public:
    G1_VECTOR C0;
    std::vector<G1_VECTOR> C1;
    std::vector<G1_VECTOR> C2;
};

// clé de déchiffrement pour un vecteur y
class KSW08SecretKey : public OpenFESecretKey {
public:
    G2_VECTOR K;
    std::vector<G2_VECTOR> K1;
    std::vector<G2_VECTOR> K2;
};

// schéma à prédicat produit scalaire KSW08 sur DPVS
class OpenFEContextKSW08 : public OpenFEContext {
private:
    size_t ell;
    BPGroup group;
    ZP ord;

    G1_VECTOR g1_gp;
    G1_VECTOR g1_gq;
    G1_VECTOR g1_gr;

    G2_VECTOR g2_gp;
    G2_VECTOR g2_gq;
    G2_VECTOR g2_gr;

public:
    OpenFEContextKSW08(size_t vector_len); // construit les vecteurs de base P/Q/R

    ~OpenFEContextKSW08() override = default;

    void setup(OpenFEMPK &mpk, OpenFEMSK &msk) override; // génère mpk/msk
    void encrypt(const OpenFEMPK &mpk, const std::vector<ZP> &x, OpenFECiphertext &ct) override;
    void keygen(const OpenFEMSK &msk, const std::vector<ZP> &y, OpenFESecretKey &sk) override;
    bool decrypt(const OpenFESecretKey &sk, const OpenFECiphertext &ct) override; // vrai si <x,y> = 0
};

}

#endif
