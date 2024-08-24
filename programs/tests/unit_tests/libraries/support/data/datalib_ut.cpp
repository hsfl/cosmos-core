#include "support/datalib.h"
#include "gtest/gtest.h"

// No tests implemented yet
TEST(DatalibTest, Placeholder)
{
    EXPECT_EQ(1, 1);
}

// Successfully compresses and moves a file when compress is true and timeout is sufficient
TEST(DatalibTest, File_is_compressed_and_moved_when_compress_is_true_and_timeout_is_sufficient) {
    string oldpath = "testfile.txt";
    string newpath = "movedfile.txt";
    bool compress = true;
    double timeout = 5.0;

    // Create a dummy file to simulate the old file
    FILE *file = fopen(oldpath.c_str(), "wb");
    ASSERT_NE(file, nullptr);
    const char *data = "This is a test file.";
    fwrite(data, sizeof(char), strlen(data), file);
    fclose(file);

    int32_t result = log_move_file(oldpath, newpath, compress, timeout);

    ASSERT_EQ(result, 0);
    newpath += ".gz";
    ASSERT_NE(fopen(newpath.c_str(), "rb"), nullptr);
    remove(newpath.c_str());
}

// Since folders cannot be compressed, the function should at least time out
TEST(DatalibTest, Folder_compress_and_move_times_out) {
    string oldpath = "testdir";
    string newpath = "moveddir";
    bool compress = true;
    double timeout = 5.0;

    // Create a dummy file to simulate the old file
    bool mkdir_result = (COSMOS_MKDIR(oldpath.c_str(), 00777) == 0  || errno == EEXIST);
    ASSERT_EQ(mkdir_result, true);

    int32_t result = log_move_file(oldpath, newpath, compress, timeout);

    ASSERT_EQ(result, GENERAL_ERROR_TIMEOUT);
    remove(oldpath.c_str());
}
