#ifndef OPENFE_CONTEXTALS16_H
#define OPENFE_CONTEXTALS16_H

#include "openfe/context.h"
#include "bpdpvs.h"
#include <vector>

namespace openfe {

class ALS16MPK : public OpenFEMPK {
public:
    G1 g;
    G1 h;
    std::vector<G1> h_i;
};

class ALS16MSK : public OpenFEMSK {
public:
    std::vector<ZP> s;
    std::vector<ZP> t;
};

class ALS16Ciphertext : public OpenFECiphertext {
public:
    G1 C;
    G1 D;
    std::vector<G1> E;
};

class ALS16SecretKey : public OpenFESecretKey {
public:
    std::vector<ZP> y;
    ZP s_y;
    ZP t_y;
};

class OpenFEContextALS16 : public OpenFEContext {
private:
    size_t ell;
    long bound;
    BPGroup group;
    ZP ord;

public:
    static constexpr long DEFAULT_BOUND = 1L << 20;

    explicit OpenFEContextALS16(size_t vector_len, long bound = DEFAULT_BOUND);

    ~OpenFEContextALS16() override = default;

    void setup(OpenFEMPK &mpk, OpenFEMSK &msk) override;
    void encrypt(const OpenFEMPK &mpk, const std::vector<long> &x, OpenFECiphertext &ct) override;
    void keygen(const OpenFEMSK &msk, const std::vector<long> &y, OpenFESecretKey &sk) override;
    long decrypt(const OpenFEMPK &mpk, const OpenFESecretKey &sk, const OpenFECiphertext &ct) override;
};

}

#endif
