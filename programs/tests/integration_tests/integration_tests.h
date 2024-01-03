#ifndef __INTEGRATION_TESTS_H__
#define __INTEGRATION_TESTS_H__

#include "gtest/gtest.h"
#include "file/file_it.h"

int run_all_integration_tests(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
