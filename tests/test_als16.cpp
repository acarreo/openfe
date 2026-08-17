#include "openfe.h"
#include "testutil.h"
#include <gtest/gtest.h>

using namespace openfe;

TEST(ALS16Test, EncryptDecrypt) {
    auto scheme = OpenFE_createContext(OpenFE_SCHEME_ALS16, 4);
    ALS16MPK mpk;
    ALS16MSK msk;
    scheme->setup(mpk, msk);

    std::vector<long> x = {1, 2, 3, 4};
    std::vector<long> y = {2, -1, 0, 0};
    std::vector<long> y2 = {1, 1, 1, 1};

    ALS16Ciphertext ct;
    scheme->encrypt(mpk, x, ct);

    ALS16SecretKey sk, sk2;
    scheme->keygen(msk, y, sk);
    scheme->keygen(msk, y2, sk2);

    EXPECT_EQ(scheme->decrypt(mpk, sk, ct), 0);
    EXPECT_EQ(scheme->decrypt(mpk, sk2, ct), 10);
}

TEST(ALS16Test, SerializeDeserialize) {
    ZP ord = openfe_test::groupOrder();

    auto scheme = OpenFE_createContext(OpenFE_SCHEME_ALS16, 4);
    ALS16MPK mpk;
    ALS16MSK msk;
    scheme->setup(mpk, msk);

    std::vector<long> x = {1, 2, 3, 4};
    std::vector<long> y = {1, 1, 1, 1};

    ALS16Ciphertext ct;
    scheme->encrypt(mpk, x, ct);
    ALS16SecretKey sk;
    scheme->keygen(msk, y, sk);

    ALS16MPK mpk2;
    mpk2.g = openfe_test::roundTrip(mpk.g);
    EXPECT_TRUE(mpk2.g == mpk.g);
    mpk2.h = openfe_test::roundTrip(mpk.h);
    EXPECT_TRUE(mpk2.h == mpk.h);
    mpk2.h_i.resize(mpk.h_i.size());
    for (size_t i = 0; i < mpk.h_i.size(); ++i) {
        mpk2.h_i[i] = openfe_test::roundTrip(mpk.h_i[i]);
        EXPECT_TRUE(mpk2.h_i[i] == mpk.h_i[i]);
    }

    ALS16Ciphertext ct2;
    ct2.C = openfe_test::roundTrip(ct.C);
    ct2.D = openfe_test::roundTrip(ct.D);
    EXPECT_TRUE(ct2.C == ct.C);
    EXPECT_TRUE(ct2.D == ct.D);
    ct2.E.resize(ct.E.size());
    for (size_t i = 0; i < ct.E.size(); ++i) {
        ct2.E[i] = openfe_test::roundTrip(ct.E[i]);
        EXPECT_TRUE(ct2.E[i] == ct.E[i]);
    }

    ALS16SecretKey sk2;
    sk2.s_y = openfe_test::roundTripZP(sk.s_y, ord);
    sk2.t_y = openfe_test::roundTripZP(sk.t_y, ord);
    EXPECT_TRUE(sk2.s_y == sk.s_y);
    EXPECT_TRUE(sk2.t_y == sk.t_y);
    sk2.y.resize(sk.y.size());
    for (size_t i = 0; i < sk.y.size(); ++i) {
        sk2.y[i] = openfe_test::roundTripZP(sk.y[i], ord);
        EXPECT_TRUE(sk2.y[i] == sk.y[i]);
    }

    EXPECT_EQ(scheme->decrypt(mpk2, sk2, ct2), 10);
}
