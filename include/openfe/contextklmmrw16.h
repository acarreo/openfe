#ifndef OPENFE_CONTEXTKLMMRW16_H
#define OPENFE_CONTEXTKLMMRW16_H

#include "openfe/context.h"
#include "bpdpvs.h"
#include <vector>

namespace openfe {

class KLMMRW16MPK : public OpenFEMPK {
public:
    G2 g2;
    std::vector<std::vector<ZP>> Bstar;
};

class KLMMRW16MSK : public OpenFEMSK {
public:
    G1 g1;
    std::vector<std::vector<ZP>> B;
    ZP detB;
};

class KLMMRW16Ciphertext : public OpenFECiphertext {
public:
    G2 C1;
    std::vector<G2> C2;
};

class KLMMRW16SecretKey : public OpenFESecretKey {
public:
    G1 K1;
    std::vector<G1> K2;
};

class OpenFEContextKLMMRW16 : public OpenFEContext {
private:
    size_t ell;
    long bound;
    BPGroup group;
    ZP ord;

public:
    static constexpr long DEFAULT_BOUND = 1L << 20;

    explicit OpenFEContextKLMMRW16(size_t vector_len, long bound = DEFAULT_BOUND);

    ~OpenFEContextKLMMRW16() override = default;

    void setup(OpenFEMPK &mpk, OpenFEMSK &msk) override;
    void encrypt(const OpenFEMPK &mpk, const std::vector<long> &x, OpenFECiphertext &ct) override;
    void keygen(const OpenFEMSK &msk, const std::vector<long> &y, OpenFESecretKey &sk) override;
    long decrypt(const OpenFEMPK &mpk, const OpenFESecretKey &sk, const OpenFECiphertext &ct) override;
};

}

#endif
