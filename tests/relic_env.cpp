#include "bpdpvs.h"
#include <gtest/gtest.h>

namespace {

class RelicEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        ASSERT_EQ(core_init(), RLC_OK);
        ASSERT_EQ(pc_param_set_any(), RLC_OK);
    }
    void TearDown() override { core_clean(); }
};

::testing::Environment *const relicEnv = ::testing::AddGlobalTestEnvironment(new RelicEnvironment);

}
