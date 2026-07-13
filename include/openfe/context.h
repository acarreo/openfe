#ifndef OPENFE_CONTEXT_H
#define OPENFE_CONTEXT_H

#include "bpdpvs.h"
#include <vector>

namespace openfe {

class OpenFEMPK {
public:
    virtual ~OpenFEMPK() = default;
};

class OpenFEMSK {
public:
    virtual ~OpenFEMSK() = default;
};

class OpenFECiphertext {
public:
    virtual ~OpenFECiphertext() = default;
};

class OpenFESecretKey {
public:
    virtual ~OpenFESecretKey() = default;
};

class OpenFEContext {
public:
    OpenFEContext() = default;
    virtual ~OpenFEContext() = default;

    virtual void setup(OpenFEMPK &mpk, OpenFEMSK &msk) = 0;
    virtual void encrypt(const OpenFEMPK &mpk, const std::vector<ZP> &x, OpenFECiphertext &ct) = 0;
    virtual void keygen(const OpenFEMSK &msk, const std::vector<ZP> &y, OpenFESecretKey &sk) = 0;
    virtual bool decrypt(const OpenFESecretKey &sk, const OpenFECiphertext &ct) = 0;
};

}

#endif
