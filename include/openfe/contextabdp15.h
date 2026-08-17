#ifndef OPENFE_CONTEXTABDP15_H
#define OPENFE_CONTEXTABDP15_H

#include "openfe/context.h"
#include "bpdpvs.h"
#include <vector>

namespace openfe {

class ABDP15MPK : public OpenFEMPK {
public:
    G1 g;
    std::vector<G1> h;
};

class ABDP15MSK : public OpenFEMSK {
public:
    std::vector<ZP> s;
};

class ABDP15Ciphertext : public OpenFECiphertext {
public:
    G1 ct0;
    std::vector<G1> ct;
};

class ABDP15SecretKey : public OpenFESecretKey {
public:
    std::vector<ZP> y;
    ZP sk_y;
};

class OpenFEContextABDP15 : public OpenFEContext {
private:
    size_t ell;
    long bound;
    BPGroup group;
    ZP ord;

public:
    static constexpr long DEFAULT_BOUND = 1L << 20;

    explicit OpenFEContextABDP15(size_t vector_len, long bound = DEFAULT_BOUND);

    ~OpenFEContextABDP15() override = default;

    void setup(OpenFEMPK &mpk, OpenFEMSK &msk) override;
    void encrypt(const OpenFEMPK &mpk, const std::vector<long> &x, OpenFECiphertext &ct) override;
    void keygen(const OpenFEMSK &msk, const std::vector<long> &y, OpenFESecretKey &sk) override;
    long decrypt(const OpenFEMPK &mpk, const OpenFESecretKey &sk, const OpenFECiphertext &ct) override;
};

}

#endif
