#ifndef __TEST_UPDATE_CHAINS_TESTS
#define __TEST_UPDATE_CHAINS_TESTS

#include "support/configCosmos.h"
#include "gtest/gtest.h"

int run_all_tests(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(SanityCheck, ObviouslyTrue)   { EXPECT_EQ(0.0, 0.0); }


#endif
