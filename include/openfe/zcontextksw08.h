#ifndef OPENFE_ZCONTEXTKSW08_H
#define OPENFE_ZCONTEXTKSW08_H

#include "openfe/zcontext.h"
#include "bpdpvs.h"
#include <vector>

namespace openfe {

// clé publique : Q + H1/H2 par coordonnée
class KSW08MPK {
public:
    G1_VECTOR Q;
    std::vector<G1_VECTOR> H1;
    std::vector<G1_VECTOR> H2;

    void serialize(ByteString &result) const;
    void deserialize(const ByteString &input);
};

// secret maître : g_q + h1/h2 par coordonnée
class KSW08MSK {
public:
    G2_VECTOR g_q;
    std::vector<G2_VECTOR> h1;
    std::vector<G2_VECTOR> h2;

    void serialize(ByteString &result) const;
    void deserialize(const ByteString &input);
};

// chiffré d'un vecteur x
class KSW08Ciphertext {
public:
    G1_VECTOR C0;
    std::vector<G1_VECTOR> C1;
    std::vector<G1_VECTOR> C2;

    void serialize(ByteString &result) const;
    void deserialize(const ByteString &input);
};

// clé de déchiffrement pour un vecteur y
class KSW08SecretKey {
public:
    G2_VECTOR K;
    std::vector<G2_VECTOR> K1;
    std::vector<G2_VECTOR> K2;

    void serialize(ByteString &result) const;
    void deserialize(const ByteString &input);
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

    void setup(KSW08MPK &mpk, KSW08MSK &msk); // génère mpk/msk
    void encrypt(const KSW08MPK &mpk, const std::vector<ZP> &x, KSW08Ciphertext &ct);
    void keygen(const KSW08MSK &msk, const std::vector<ZP> &y, KSW08SecretKey &sk);
    bool decrypt(const KSW08SecretKey &sk, const KSW08Ciphertext &ct); // vrai si <x,y> = 0
};

}

#endif
