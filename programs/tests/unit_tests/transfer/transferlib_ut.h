#ifndef __TRANSFERLIB_UT_H__
#define __TRANSFERLIB_UT_H__

#include "support/transferlib.h"
#include "gtest/gtest.h"

void compare_file_progress(const file_progress& fp, const file_progress& fp2, size_t LINE)
{
    EXPECT_EQ(fp.chunk_start, fp2.chunk_start) << "transferlib_ut.h:" << LINE;
    EXPECT_EQ(fp.chunk_end, fp2.chunk_end) << "transferlib_ut.h:" << LINE;
}

void compare_tx_progress(const vector<file_progress>& tp, const vector<file_progress>& tp2, size_t LINE)
{
    EXPECT_EQ(tp.size(), tp2.size());
    for (size_t i=0; i<tp.size(); ++i) {
        compare_file_progress(tp[i], tp2[i], LINE);
    }
}

TEST(TransferlibTest, add_chunk)
{
    // Keep mock file_info here
    vector<file_progress> mfile_info;

    tx_progress tx;
    tx.file_size = 1000;
    tx.total_bytes = 0;
    tx.file_info.clear();

    file_progress tp;
    bool ret;

    // Add initial chunk
    tp = {0, 200};
    ret = add_chunk(tx, tp);
    mfile_info = {tp};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 201);

    // Add duplicate chunk
    ret = add_chunk(tx, tp);
    EXPECT_EQ(ret, false);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 201);

    // Add a chunk with chunk start overlapping between existing chunk
    tp = {50, 300};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 301);

    // Add a chunk that does not overlap at all
    tp = {700, 800};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {700, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 301+101);

    // Add a chunk inbetween that does not overlap
    tp = {400, 500};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {400, 500}, {700, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 301+101+101);

    // Add a chunk that completely encompasses a chunk
    tp = {350, 550};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {350, 550}, {700, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 301+201+101);

    // Add a chunk that overlaps on the end with another chunk
    tp = {600, 750};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {350, 550}, {600, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 301+201+201);

    // Add a chunk at the end of the queue, no overlaps
    tp = {900, 999};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {350, 550}, {600, 800}, {900, 999}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 301+201+201+100);

    // Add a multi-chunk overlapping chunk
    tp = {300, 400};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 400}, {350, 550}, {600, 800}, {900, 999}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 401+201+201+100);
}

TEST(TransferlibTest, merge_chunks_overlap)
{
    // Keep mock file_info here
    vector<file_progress> mfile_info;

    tx_progress tx;
    tx.file_size = 1000;
    tx.total_bytes = 401+201+201+100;
    tx.file_info = {{900, 999}, {0, 400}, {350, 550}, {600, 800}};
    mfile_info = {{0, 400}, {350, 550}, {600, 800}, {900, 999}};
    // Test sorting is correct
    sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);

    // Merge chunk
    merge_chunks_overlap(tx);
    mfile_info = {{0, 550}, {600, 800}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 551+201+100);

    
    // Chunks that go beyond file size should be evicted
    tx.file_info = {{600, 1200}, {100,200}};
    merge_chunks_overlap(tx);
    mfile_info = {{100,200}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 101);

    // Test that file_size 0 (no metadata received yet) and erasing single element is safe
    tx.file_size = 0;
    tx.file_info = {{100, 200}};
    mfile_info = {};
    merge_chunks_overlap(tx);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, 0);
}

TEST(TransferlibTest, reqdata_packets_are_created_correctly)
{
    // Keep mock file_info here
    vector<file_progress> holes;
    size_t packet_size = 214;
    int32_t current_start = 0;
    for (size_t i=0; i < (packet_size*3) / sizeof(file_progress); ++i)
    {
        holes.push_back({current_start, current_start + 50});
        current_start += 52;
    }
    vector<PacketComm> reqdata_packets;
    serialize_reqdata(reqdata_packets, 1, 2, 127, holes, packet_size);
    size_t current_hole_idx = 0;
    for (auto& packet : reqdata_packets)
    {
        packet_struct_reqdata reqdata;
        deserialize_reqdata(packet.data, reqdata);
        for (auto& hole : reqdata.holes)
        {
            compare_file_progress(holes[current_hole_idx], hole, __LINE__);
            ++current_hole_idx;
        }
    }
    EXPECT_EQ(current_hole_idx, holes.size());
}

#endif // End __TRANSFERLIB_UT_H__
