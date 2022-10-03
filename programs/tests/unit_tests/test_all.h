#ifndef __TEST_ALL_H_
#define __TEST_ALL_H_

#include "gtest/gtest.h"

int run_all_tests(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(SanityCheck, ObviouslyTrue)   { EXPECT_EQ(0.0, 0.0); }

#endif
