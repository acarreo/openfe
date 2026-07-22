#include "openfe.h"
#include "testutil.h"
#include <gtest/gtest.h>

using namespace openfe;

TEST(KLMMRW16Test, EncryptDecrypt) {
    auto scheme = OpenFE_createContext(OpenFE_SCHEME_KLMMRW16, 4);
    KLMMRW16MPK mpk;
    KLMMRW16MSK msk;
    scheme->setup(mpk, msk);

    std::vector<long> x = {1, 2, 3, 4};
    std::vector<long> y = {2, -1, 0, 0};
    std::vector<long> y2 = {1, 1, 1, 1};

    KLMMRW16Ciphertext ct;
    scheme->encrypt(mpk, x, ct);

    KLMMRW16SecretKey sk, sk2;
    scheme->keygen(msk, y, sk);
    scheme->keygen(msk, y2, sk2);

    EXPECT_EQ(scheme->decrypt(mpk, sk, ct), 0);
    EXPECT_EQ(scheme->decrypt(mpk, sk2, ct), 10);
}

TEST(KLMMRW16Test, SerializeDeserialize) {
    ZP ord = openfe_test::groupOrder();

    auto scheme = OpenFE_createContext(OpenFE_SCHEME_KLMMRW16, 4);
    KLMMRW16MPK mpk;
    KLMMRW16MSK msk;
    scheme->setup(mpk, msk);

    std::vector<long> x = {1, 2, 3, 4};
    std::vector<long> y = {1, 1, 1, 1};

    KLMMRW16Ciphertext ct;
    scheme->encrypt(mpk, x, ct);
    KLMMRW16SecretKey sk;
    scheme->keygen(msk, y, sk);

    KLMMRW16MPK mpk2;
    mpk2.g2 = openfe_test::roundTrip(mpk.g2);
    EXPECT_TRUE(mpk2.g2 == mpk.g2);
    mpk2.Bstar.resize(mpk.Bstar.size());
    for (size_t i = 0; i < mpk.Bstar.size(); ++i) {
        mpk2.Bstar[i].resize(mpk.Bstar[i].size());
        for (size_t j = 0; j < mpk.Bstar[i].size(); ++j) {
            mpk2.Bstar[i][j] = openfe_test::roundTripZP(mpk.Bstar[i][j], ord);
            EXPECT_TRUE(mpk2.Bstar[i][j] == mpk.Bstar[i][j]);
        }
    }

    KLMMRW16Ciphertext ct2;
    ct2.C1 = openfe_test::roundTrip(ct.C1);
    EXPECT_TRUE(ct2.C1 == ct.C1);
    ct2.C2.resize(ct.C2.size());
    for (size_t i = 0; i < ct.C2.size(); ++i) {
        ct2.C2[i] = openfe_test::roundTrip(ct.C2[i]);
        EXPECT_TRUE(ct2.C2[i] == ct.C2[i]);
    }

    KLMMRW16SecretKey sk2;
    sk2.K1 = openfe_test::roundTrip(sk.K1);
    EXPECT_TRUE(sk2.K1 == sk.K1);
    sk2.K2.resize(sk.K2.size());
    for (size_t i = 0; i < sk.K2.size(); ++i) {
        sk2.K2[i] = openfe_test::roundTrip(sk.K2[i]);
        EXPECT_TRUE(sk2.K2[i] == sk.K2[i]);
    }

    EXPECT_EQ(scheme->decrypt(mpk2, sk2, ct2), 10);
}
