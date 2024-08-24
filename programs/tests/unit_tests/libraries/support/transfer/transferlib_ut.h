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
    size_t size = std::min(tp.size(), tp2.size());
    for (size_t i=0; i<size; ++i) {
        compare_file_progress(tp[i], tp2[i], LINE);
    }
}

size_t sum_fp(const vector<file_progress>& tp)
{
    size_t sum = 0;
    for (const auto& hole : tp)
    {
        sum += (hole.chunk_end - hole.chunk_start) + 1;
    }
    return sum;
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
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add duplicate chunk
    ret = add_chunk(tx, tp);
    EXPECT_EQ(ret, false);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a chunk with chunk start overlapping between existing chunk
    tp = {50, 300};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a chunk that does not overlap at all
    tp = {700, 800};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {700, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a chunk inbetween that does not overlap
    tp = {400, 500};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {400, 500}, {700, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a chunk that completely encompasses a chunk
    tp = {350, 550};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {350, 550}, {700, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a chunk that overlaps on the end with another chunk
    tp = {600, 750};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {350, 550}, {600, 800}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a chunk at the end of the queue, no overlaps
    tp = {900, 999};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 300}, {350, 550}, {600, 800}, {900, 999}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a multi-chunk overlapping chunk
    tp = {300, 400};
    ret = add_chunk(tx, tp);
    mfile_info = {{0, 400}, {350, 550}, {600, 800}, {900, 999}};
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));
}

TEST(TransferlibTest, add_chunk_with_start)
{
    // Keep mock file_info here
    vector<file_progress> mfile_info = {{300, 399}, {600, 799}, {900, 999}};
    bool is_start = true;
    bool is_end = false;

    // Modify this, compare against mock
    tx_progress tx;
    tx.file_size = 1000;
    tx.total_bytes = 0;
    tx.file_info.clear();
    // Dummy hole to add
    file_progress hole;
    bool ret;

    // Test start that does not overlap ///////////////////////////////////////////////////
    // Add initial chunks
    ret = add_chunks(tx, mfile_info, 0);
    EXPECT_EQ(ret, true);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start-hole that is at the front, no overlaps
    hole = {100, 199};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {600, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start-hole that starts and ends before a hole in the middle
    hole = {225, 249};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{225, 249}, {300, 399}, {600, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start-hole that is at the front and overlaps with the first hole
    hole = {50, 230};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{50, 249}, {300, 399}, {600, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start-hole that starts before a hole in the middle of the vector,
    // and ends after that hole, but does not overlap with the next.
    hole = {350, 450};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{350, 450}, {600, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start-hole that starts before a hole in the middle of the vector,
    // but overlaps with the next.
    hole = {500, 650};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{500, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Test start overlaps in middle of a hole ////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {600, 799}, {900, 999}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    // Add a start-hole that starts in the middle of a hole
    // that is in the middle of the vector, and ends within that same hole
    hole = {325, 374};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{325, 399}, {600, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start-hole that starts in the middle of a hole
    // that is in the middle of the vector, but does not overlap with the next.
    hole = {350, 449};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{350, 449}, {600, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start-hole that starts in the middle of a hole
    // that is in the middle of the vector, has an end beyond that hole,
    // but does not overlap with the next.
    hole = {700, 950};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{700, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Test start-hole at the end, no overlap /////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);
    // Add start-hole that starts at the end, no overlap
    hole = {900, 999};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));
}

TEST(TransferlibTest, add_chunk_with_end)
{
    // Keep mock file_info here
    vector<file_progress> mfile_info;
    bool is_start = false;
    bool is_end = true;

    // Modify this, compare against mock
    tx_progress tx;
    tx.file_size = 1000;
    tx.total_bytes = 0;
    tx.file_info.clear();
    // Dummy hole to add
    file_progress hole;
    bool ret;

    // Test start that does not overlap ///////////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {600, 799}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    // Add an end-hole that is at the end, no overlaps
    hole = {900, 999};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {600, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add an end-hole that starts and ends before a hole in the middle
    hole = {825, 849};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {600, 799}, {825, 849}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add an end-hole that is at the end and overlaps with the last hole
    hole = {830, 999};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {600, 799}, {825, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add an end-hole that starts before a hole in the middle of the vector,
    // but overlaps with the next.
    hole = {550, 650};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {550, 650}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add an end-hole that starts before a hole in the middle of the vector,
    // and ends after that hole, but does not overlap with the next.
    hole = {525, 750};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {525, 750}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Test start overlaps in middle of a hole ////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 999}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    // Add an end-hole that starts in the middle of a hole
    // that is in the middle of the vector, and ends within that same hole
    hole = {725, 774};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 774}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add an end-hole that starts in the middle of a hole
    // that is in the middle of the vector, has an end beyond that hole,
    // but does not overlap with the next.
    hole = {550, 649};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {500, 649}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add an end-hole that starts in the middle of a hole
    // that is in the middle of the vector, and overlaps with the next
    hole = {150, 349};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 349}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Test end-hole at the end, no overlap ///////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);
    // Add end-hole that starts at the end, no overlap
    hole = {900,999};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

}

TEST(TransferlibTest, add_chunk_with_start_and_end)
{
    // Keep mock file_info here
    vector<file_progress> mfile_info;
    bool is_start = true;
    bool is_end = true;

    // Modify this, compare against mock
    tx_progress tx;
    tx.file_size = 1000;
    tx.total_bytes = 0;
    tx.file_info.clear();
    // Dummy hole to add
    file_progress hole;
    bool ret;

    // Test start that does not overlap ///////////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    // Add a start&end-hole that is at the end, no overlaps
    hole = {900, 999};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start&end-hole that starts and ends before a hole in the middle //////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 999}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    hole = {625, 649};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{625, 649}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start&end-hole that is at the end and overlaps with the last hole ////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 919}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    hole = {830, 999};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{830, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start&end-hole that starts before a hole in the middle of the vector, ////////
    // but overlaps with the next. ////////////////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 999}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    hole = {550, 750};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{550, 750}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Test start overlaps in middle of a hole ////////////////////////////////////////////
    // Add a start&end-hole that starts in the middle of a hole ///////////////////////////
    // that is in the middle of the vector, and ends within that same hole ////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 999}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);
    
    hole = {525, 574};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{525, 574}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start&end-hole that starts in the middle of a hole ///////////////////////////
    // that is in the middle of the vector, has an end beyond that hole, //////////////////
    // but does not overlap with the next./////////////////////////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 999}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    hole = {550, 649};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{550, 649}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Add a start&end-hole that starts in the middle of a hole ///////////////////////////
    // that is in the middle of the vector, and overlaps with the next ////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}, {900, 999}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    hole = {350, 549};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{350, 549}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));


    // Test end-hole at the end, no overlap ///////////////////////////////////////////////
    // Add start&end-hole that starts at the end, no overlap //////////////////////////////
    // Reinitialize
    mfile_info = {{100, 199}, {300, 399}, {500, 599}, {700, 799}};
    tx.file_info = mfile_info;
    tx.total_bytes = sum_fp(mfile_info);

    hole = {900,999};
    ret = add_chunk(tx, hole, is_start, is_end);
    EXPECT_EQ(ret, true);
    mfile_info = {{900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

}


TEST(TransferlibTest, add_chunks)
{
    // Keep mock file_info here
    vector<file_progress> holes;
    
    // First create a bunch of holes, and reqdata packets
    size_t packet_size = 214;
    size_t num_reqdata_packets = 4;
    PACKET_FILE_SIZE_TYPE current_start = 0;
    // Initialize holes = {0, 99}, {200, 299}, {400, 499}, ..., etc.
    for (size_t i=0; i < (packet_size*num_reqdata_packets) / sizeof(file_progress); ++i)
    {
        holes.push_back({current_start, current_start + 99});
        current_start += 200;
    }
    vector<PacketComm> reqdata_packets;
    serialize_reqdata(reqdata_packets, 1, 2, 127, 0, holes, packet_size);

    // Modify this, compare against mock
    tx_progress tx;
    tx.file_size = current_start;
    tx.total_bytes = 0;
    tx.file_info.clear();

    // Deserialize each reqdata packet
    for (size_t i=0; i<reqdata_packets.size(); ++i)
    {
        packet_struct_reqdata reqdata;
        int32_t start_end_signifier = deserialize_reqdata(reqdata_packets[i].data, reqdata);
        // First packet should contain start of holes
        if (i == 0)
        {
            EXPECT_EQ(start_end_signifier & 0x1, 0x1);
        }
        // Last packet should contain end of holes
        if (i == reqdata_packets.size() - 1)
        {
            EXPECT_EQ(start_end_signifier & 0x2, 0x2);
        }
        add_chunks(tx, reqdata.holes, start_end_signifier);
    }
    compare_tx_progress(tx.file_info, holes, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(holes));

    // Add two end pieces
    vector<file_progress> start_end_holes = { {300,549}, {900,999} };
    reqdata_packets.clear();
    serialize_reqdata(reqdata_packets, 1, 2, 127, 0, start_end_holes, packet_size);
    packet_struct_reqdata reqdata;
    int32_t start_end_signifier = deserialize_reqdata(reqdata_packets[0].data, reqdata);
    add_chunks(tx, reqdata.holes, start_end_signifier);
    // Compare
    // { {0,99}, {200,299}, {400,499}, {600,499}, {800,899}, {1000,1999}, ..., etc. }
    // + { {300,549}, {900,999} } (has start and end!)
    // = { {300,549}, {600,699}, {800,999} }
    holes = { {300,549}, {600,699}, {800,999} };
    compare_tx_progress(tx.file_info, holes, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(holes));
}

TEST(TransferlibTest, merge_chunks_overlap)
{
    // Keep mock file_info here
    vector<file_progress> mfile_info;

    tx_progress tx;
    tx.file_size = 1000;
    tx.file_info = {{900, 999}, {0, 400}, {350, 550}, {600, 800}};
    mfile_info = {{0, 400}, {350, 550}, {600, 800}, {900, 999}};
    // Test sorting is correct
    sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);

    // Merge chunk
    merge_chunks_overlap(tx);
    mfile_info = {{0, 550}, {600, 800}, {900, 999}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    
    // Chunks that go beyond file size should be evicted
    tx.file_info = {{600, 1200}, {100,200}};
    merge_chunks_overlap(tx);
    mfile_info = {{100,200}};
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));

    // Test that file_size 0 (no metadata received yet) does not erase current progress
    tx.file_size = 0;
    tx.file_info = {{100, 200}};
    mfile_info = {{100,200}};
    merge_chunks_overlap(tx);
    compare_tx_progress(tx.file_info, mfile_info, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(mfile_info));
}

TEST(TransferlibTest, reqdata_packets_with_start_end_signifier)
{
    // Keep mock file_info here
    vector<file_progress> holes;
    size_t packet_size = 214;
    uint32_t current_start = 0;
    for (size_t i=0; i < (packet_size*3) / sizeof(file_progress); ++i)
    {
        holes.push_back({current_start, current_start + 50});
        current_start += 52;
    }
    vector<PacketComm> reqdata_packets;
    serialize_reqdata(reqdata_packets, 1, 2, 127, 0, holes, packet_size);
    size_t current_hole_idx = 0;
    for (size_t i=0; i<reqdata_packets.size(); ++i)
    {
        packet_struct_reqdata reqdata;
        int32_t start_end_signifier = deserialize_reqdata(reqdata_packets[i].data, reqdata);
        uint8_t signifier = reqdata_packets[i].data[reqdata_packets[i].data.size()-1];
        EXPECT_EQ(start_end_signifier, signifier);
        // First packet should contain start of holes
        if (i == 0)
        {
            EXPECT_EQ(signifier & 0x1, 0x1);
        }
        // Last packet should contain end of holes
        if (i == reqdata_packets.size() - 1)
        {
            EXPECT_EQ(signifier & 0x2, 0x2);
        }
        for (auto& hole : reqdata.holes)
        {
            compare_file_progress(holes[current_hole_idx], hole, __LINE__);
            ++current_hole_idx;
        }
    }
    EXPECT_EQ(current_hole_idx, holes.size());
}

TEST(TransferlibTest, reqdata_packets)
{
    // Keep mock file_info here
    vector<file_progress> mock_missing;
    vector<file_progress> missing;

    tx_progress tx, tx2;
    tx.sentmeta = tx2.sentmeta = true;
    tx.file_size = tx2.file_size = 1000;

    // Chunk has a whole in the middle and ends
    tx.file_info = {{100, 249}, {750,899}};
    mock_missing = {{0,99}, {250,749}, {900,999}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, false);

    vector<PacketComm> packets;
    PACKET_NODE_ID_TYPE self_node_id = 1;
    PACKET_NODE_ID_TYPE orig_node_id = 10;
    uint8_t tx_id = 172;
    uint16_t file_crc = 99;
    PACKET_CHUNK_SIZE_TYPE packet_size = 200;
    serialize_reqdata(packets, self_node_id, orig_node_id, tx_id, file_crc, missing, packet_size);
    
    // Receive
    for (auto packet : packets)
    {
        packet_struct_reqdata reqdata;
        int32_t start_end_signifier = deserialize_reqdata(packet.data, reqdata);
        EXPECT_EQ(unsigned(reqdata.header.node_id), unsigned(self_node_id));
        
        EXPECT_GT(reqdata.header.tx_id, 0);
        if (tx_id <= 0)
        {
            continue;
        }

        EXPECT_GE(start_end_signifier, 0);
        if (start_end_signifier < 0)
        {
            continue;
        }

        // Add this chunk to the queue
        bool updated = add_chunks(tx2, reqdata.holes, start_end_signifier);
        EXPECT_EQ(updated, true);
        
        // Recalculate chunks
        merge_chunks_overlap(tx2);
    }
    compare_tx_progress(tx2.file_info, mock_missing, __LINE__);
}

TEST(TransferlibTest, reqdata_packets_no_holes)
{
    // TODO: rethink this test.
    // Currently, serialize_reqdata wouldn't be called in transferclass if
    // missing is empty. On the other hand, serialize_reqdata probably shouldn't
    // be returning anything if missing is empty anyway.

    // Keep mock file_info here
    vector<file_progress> mock_missing;
    vector<file_progress> missing;

    tx_progress tx, tx2;
    tx.sentmeta = tx2.sentmeta = true;
    tx.file_size = tx2.file_size = 1000;
    tx2.file_info = {{0,999}};

    tx.file_info = {{0,999}};
    mock_missing = {};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, true);

    vector<PacketComm> packets;
    PACKET_NODE_ID_TYPE self_node_id = 1;
    PACKET_NODE_ID_TYPE orig_node_id = 10;
    uint8_t tx_id = 172;
    uint16_t file_crc = 99;
    PACKET_CHUNK_SIZE_TYPE packet_size = 200;
    serialize_reqdata(packets, self_node_id, orig_node_id, tx_id, file_crc, missing, packet_size);
    
    // Receive
    for (auto packet : packets)
    {
        packet_struct_reqdata reqdata;
        int32_t start_end_signifier = deserialize_reqdata(packet.data, reqdata);
        EXPECT_GE(start_end_signifier, 0);
        if (start_end_signifier < 0)
        {
            continue;
        }
        
        EXPECT_GT(reqdata.header.tx_id, 0);
        if (tx_id <= 0)
        {
            continue;
        }

        // Add this chunk to the queue
        // bool updated = add_chunks(tx2, reqdata.holes, start_end_signifier);
        // EXPECT_EQ(updated, true); // See todo comment at the top of the test
        
        // Recalculate chunks
        merge_chunks_overlap(tx2);
    }
    // compare_tx_progress(tx2.file_info, mock_missing, __LINE__); // See todo comment at the top of the test
}

TEST(TransferlibTest, find_chunks_missing)
{
    // Keep mock file_info here
    vector<file_progress> mock_missing;
    vector<file_progress> missing;

    tx_progress tx;

    // If METADATA was not received yet, then file_size is not
    // known, so return empty
    tx.sentmeta = false;
    tx.file_size = 1000;
    tx.file_info = {{0, 499}};
    mock_missing = {};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);

    // No received chunks
    tx.sentmeta = true;
    tx.file_info = {};
    mock_missing = {{0, 999}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, false);

    // Chunk is flush against beginning
    tx.sentmeta = true;
    tx.file_info = {{0, 499}};
    mock_missing = {{500, 999}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, false);

    // Chunk is flush against the end
    tx.file_info = {{500, 999}};
    mock_missing = {{0, 499}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, false);

    // Chunk has a whole to left and right
    tx.file_info = {{250, 749}};
    mock_missing = {{0, 249}, {750, 999}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, false);

    // Chunk has a whole in the middle
    tx.file_info = {{0, 249}, {750,999}};
    mock_missing = {{250,749}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, false);

    // Chunk has a whole in the middle and ends
    tx.file_info = {{100, 249}, {750,899}};
    mock_missing = {{0,99}, {250,749}, {900,999}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp(tx.file_info));
    EXPECT_EQ(tx.complete, false);

    // Chunk overlaps beyond the end of the file_size
    // Chunks that go over the file size are evicted
    tx.file_info = {{100, 249}, {750,1099}};
    mock_missing = {{0,99}, {250,999}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp({{100, 249}}));
    EXPECT_EQ(tx.complete, false);

    // Chunks completely beyond the file
    tx.file_info = {{100, 249}, {1099,1199}};
    mock_missing = {{0,99}, {250,999}};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp({{100, 249}}));
    EXPECT_EQ(tx.complete, false);

    // Chunks completely beyond the file
    tx.file_info = {{0, 600}, {500,999}};
    mock_missing = {};
    missing = find_chunks_missing(tx);
    compare_tx_progress(missing, mock_missing, __LINE__);
    EXPECT_EQ(tx.total_bytes, sum_fp({{0, 999}}));
    EXPECT_EQ(tx.complete, true);
}


TEST(TransferlibTest, clear_tx_entry)
{
    tx_entry tx;
    // Add some files
    tx.progress[1].temppath = "testfile1";
    FILE* fp;
    fp = fopen((tx.progress[1].temppath + ".meta").c_str(), "w");
    ASSERT_NE(fp, nullptr);
    fclose(fp);
    fp = nullptr;
    tx.progress[1].fp = fopen((tx.progress[1].temppath + ".file").c_str(), "w");
    ASSERT_NE(tx.progress[1].fp, nullptr);
    // File 10 will have only a .meta, no .file
    tx.progress[10].temppath = "testfile10";
    fp = fopen((tx.progress[10].temppath + ".meta").c_str(), "w");
    ASSERT_NE(fp, nullptr);
    fclose(fp);
    fp = nullptr;

    // Attempt clear
    clear_tx_entry(tx);
    // Calling it again ought to segfault if a file pointer was not properly defaulted
    clear_tx_entry(tx);
    // Look inside the folder of the test executable, no testfiles should be around if
    // clear_tx_entry properly cleaned things up
}

TEST(TransferlibTest, data_packets_are_correct)
{
    PacketComm packet;
    uint8_t orig_node_id = 1;
    PACKET_TX_ID_TYPE tx_id = 172;
    PACKET_FILE_SIZE_TYPE file_crc = 25522;
    PACKET_CHUNK_SIZE_TYPE byte_count = 150;
    PACKET_FILE_SIZE_TYPE chunk_start = 0;
    PACKET_BYTE chunk[150];
    for (size_t i=0; i<byte_count; ++i)
    {
        chunk[i] = i;
    }
    serialize_data(packet, orig_node_id, tx_id, file_crc, byte_count, chunk_start, chunk);

    packet_struct_data data;
    deserialize_data(packet.data, data);
    EXPECT_EQ(data.header.version, FILE_TRANSFER_PROTOCOL_VERSION);
    EXPECT_EQ(data.header.node_id, orig_node_id);
    EXPECT_EQ(data.header.tx_id, tx_id);
    EXPECT_EQ(data.header.file_crc, file_crc);
    EXPECT_EQ(data.byte_count, byte_count);
    EXPECT_EQ(data.chunk_start, chunk_start);
    ASSERT_EQ(data.chunk.size(), byte_count);
    bool diff = false;
    for (size_t i=0; i<byte_count; ++i)
    {
        if (data.chunk[i] != chunk[i])
        {
            diff = true;
            break;
        }
    }
    EXPECT_EQ(diff, false);
}

TEST(TransferlibTest, complete_packets_are_correct)
{
    PacketComm packet;
    uint8_t node_id = 1;
    PACKET_TX_ID_TYPE tx_id = 172;
    PACKET_FILE_SIZE_TYPE file_crc = 25522;
    serialize_complete(packet, node_id, tx_id, file_crc);

    packet_struct_complete complete;
    deserialize_complete(packet.data, complete);
    EXPECT_EQ(complete.header.version, FILE_TRANSFER_PROTOCOL_VERSION);
    EXPECT_EQ(complete.header.node_id, node_id);
    EXPECT_EQ(complete.header.tx_id, tx_id);
    EXPECT_EQ(complete.header.file_crc, file_crc);
}

TEST(TransferlibTest, reqcomplete_packets_are_correct)
{
    PacketComm packet;
    uint8_t node_id = 1;
    PACKET_TX_ID_TYPE tx_id = 172;
    PACKET_FILE_SIZE_TYPE file_crc = 25522;
    serialize_reqcomplete(packet, node_id, tx_id, file_crc);

    packet_struct_reqcomplete reqcomplete;
    deserialize_reqcomplete(packet.data, reqcomplete);
    EXPECT_EQ(reqcomplete.header.version, FILE_TRANSFER_PROTOCOL_VERSION);
    EXPECT_EQ(reqcomplete.header.node_id, node_id);
    EXPECT_EQ(reqcomplete.header.tx_id, tx_id);
    EXPECT_EQ(reqcomplete.header.file_crc, file_crc);
}

TEST(TransferlibTest, metadata_packets_are_correct)
{
    PacketComm packet;
    uint8_t node_id = 1;
    PACKET_TX_ID_TYPE tx_id = 172;
    PACKET_FILE_SIZE_TYPE file_crc = 25522;
    const string file_name = "filename";
    PACKET_FILE_SIZE_TYPE file_size = 63330;
    const string agent_name = "agentname";
    serialize_metadata(packet, node_id, tx_id, file_crc, file_name, file_size, agent_name);

    packet_struct_metadata meta;
    deserialize_metadata(packet.data, meta);
    EXPECT_EQ(meta.header.version, FILE_TRANSFER_PROTOCOL_VERSION);
    EXPECT_EQ(meta.header.node_id, node_id);
    EXPECT_EQ(meta.header.tx_id, tx_id);
    EXPECT_EQ(meta.header.file_crc, file_crc);
    EXPECT_EQ(meta.agent_name_len, agent_name.size());
    EXPECT_EQ(meta.agent_name, agent_name);
    EXPECT_EQ(meta.file_name_len, file_name.size());
    EXPECT_EQ(meta.file_name, file_name);
    EXPECT_EQ(meta.file_size, file_size);
}

TEST(TransferlibTest, reqmeta_packets_are_correct)
{
    PacketComm packet;
    uint8_t node_id = 1;
    PACKET_TX_ID_TYPE tx_id = 0;
    PACKET_FILE_SIZE_TYPE file_crc = 0;
    const string node_name = "nodename";
    vector<PACKET_TX_ID_TYPE> req_ids = {1, 2, 15, 33, 34, 56, 122, 208, 244};
    serialize_reqmeta(packet, node_id, node_name, req_ids);

    packet_struct_reqmeta reqmeta;
    deserialize_reqmeta(packet.data, reqmeta);
    EXPECT_EQ(reqmeta.header.version, FILE_TRANSFER_PROTOCOL_VERSION);
    EXPECT_EQ(reqmeta.header.node_id, node_id);
    EXPECT_EQ(reqmeta.header.tx_id, tx_id);
    EXPECT_EQ(reqmeta.header.file_crc, file_crc);
    for (auto tx_id : req_ids)
    {
        uint8_t bitshift = tx_id % 8;
        uint8_t bucket = tx_id / 8;
        EXPECT_EQ((reqmeta.tx_ids[bucket] >> bitshift) & 0x1, 1) << "tx_id: " << unsigned(tx_id);
    }
}

TEST(TransferlibTest, queue_packets_are_correct)
{
    PacketComm packet;
    uint8_t node_id = 1;
    PACKET_TX_ID_TYPE tx_id = 0;
    PACKET_FILE_SIZE_TYPE file_crc = 0;
    const string node_name = "nodename";
    vector<PACKET_TX_ID_TYPE> req_ids = {1, 2, 15, 33, 34, 56, 122, 208, 244};
    serialize_queue(packet, node_id, req_ids);

    packet_struct_queue queue;
    deserialize_queue(packet.data, queue);
    EXPECT_EQ(queue.header.version, FILE_TRANSFER_PROTOCOL_VERSION);
    EXPECT_EQ(queue.header.node_id, node_id);
    EXPECT_EQ(queue.header.tx_id, tx_id);
    EXPECT_EQ(queue.header.file_crc, file_crc);
    for (auto tx_id : req_ids)
    {
        uint8_t bitshift = tx_id % 8;
        uint8_t bucket = tx_id / 8;
        EXPECT_EQ((queue.tx_ids[bucket] >> bitshift) & 0x1, 1) << "tx_id: " << unsigned(tx_id);
    }
}

TEST(TransferlibTest, cancel_packets_are_correct)
{
    PacketComm packet;
    uint8_t node_id = 1;
    PACKET_TX_ID_TYPE tx_id = 172;
    PACKET_FILE_SIZE_TYPE file_crc = 25522;
    serialize_cancel(packet, node_id, tx_id, file_crc);

    packet_struct_cancel cancel;
    deserialize_cancel(packet.data, cancel);
    EXPECT_EQ(cancel.header.version, FILE_TRANSFER_PROTOCOL_VERSION);
    EXPECT_EQ(cancel.header.node_id, node_id);
    EXPECT_EQ(cancel.header.tx_id, tx_id);
    EXPECT_EQ(cancel.header.file_crc, file_crc);
}

#endif // End __TRANSFERLIB_UT_H__
