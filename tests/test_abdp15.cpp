#include "openfe.h"
#include "testutil.h"
#include <gtest/gtest.h>

using namespace openfe;

TEST(ABDP15Test, EncryptDecrypt) {
    auto scheme = OpenFE_createContext(OpenFE_SCHEME_ABDP15, 4);
    ABDP15MPK mpk;
    ABDP15MSK msk;
    scheme->setup(mpk, msk);

    std::vector<long> x = {1, 2, 3, 4};
    std::vector<long> y = {2, -1, 0, 0};
    std::vector<long> y2 = {1, 1, 1, 1};

    ABDP15Ciphertext ct;
    scheme->encrypt(mpk, x, ct);

    ABDP15SecretKey sk, sk2;
    scheme->keygen(msk, y, sk);
    scheme->keygen(msk, y2, sk2);

    EXPECT_EQ(scheme->decrypt(mpk, sk, ct), 0);
    EXPECT_EQ(scheme->decrypt(mpk, sk2, ct), 10);
}

TEST(ABDP15Test, SerializeDeserialize) {
    ZP ord = openfe_test::groupOrder();

    auto scheme = OpenFE_createContext(OpenFE_SCHEME_ABDP15, 4);
    ABDP15MPK mpk;
    ABDP15MSK msk;
    scheme->setup(mpk, msk);

    std::vector<long> x = {1, 2, 3, 4};
    std::vector<long> y = {1, 1, 1, 1};

    ABDP15Ciphertext ct;
    scheme->encrypt(mpk, x, ct);
    ABDP15SecretKey sk;
    scheme->keygen(msk, y, sk);

    ABDP15MPK mpk2;
    mpk2.g = openfe_test::roundTrip(mpk.g);
    EXPECT_TRUE(mpk2.g == mpk.g);
    mpk2.h.resize(mpk.h.size());
    for (size_t i = 0; i < mpk.h.size(); ++i) {
        mpk2.h[i] = openfe_test::roundTrip(mpk.h[i]);
        EXPECT_TRUE(mpk2.h[i] == mpk.h[i]);
    }

    ABDP15Ciphertext ct2;
    ct2.ct0 = openfe_test::roundTrip(ct.ct0);
    EXPECT_TRUE(ct2.ct0 == ct.ct0);
    ct2.ct.resize(ct.ct.size());
    for (size_t i = 0; i < ct.ct.size(); ++i) {
        ct2.ct[i] = openfe_test::roundTrip(ct.ct[i]);
        EXPECT_TRUE(ct2.ct[i] == ct.ct[i]);
    }

    ABDP15SecretKey sk2;
    sk2.sk_y = openfe_test::roundTripZP(sk.sk_y, ord);
    EXPECT_TRUE(sk2.sk_y == sk.sk_y);
    sk2.y.resize(sk.y.size());
    for (size_t i = 0; i < sk.y.size(); ++i) {
        sk2.y[i] = openfe_test::roundTripZP(sk.y[i], ord);
        EXPECT_TRUE(sk2.y[i] == sk.y[i]);
    }

    EXPECT_EQ(scheme->decrypt(mpk2, sk2, ct2), 10);
}
