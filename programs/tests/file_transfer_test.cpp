//! For testing the file transfer protocol implemented by transferclass and transferlib
//!
//! Usage: file_transfer_test [delete_test_dirs] [packet_size]
//! delete_test_dirs, by default true, will remove the created test directories. 0 or 1.
//! packet_size is the size of the transfer file packet. Simulate packet size restrictions of various radios.
//! Test as of this commit successful for up to packet_size=65535, didn't test above that
//! Test as of this commit successful for as lower bound packet_size=47 (a shorter node name will make this smaller by same amount)

#include "support/transferclass.h"
#include <fstream>
#include <numeric>      // for std::accumulate
#include <algorithm>    // for std::shuffle
#include <random>       // for std::default_random_machine

#define NODE1 0
#define NODE2 1

// Change size of transfer packet, defaults to 217, size of S-Band radio
PACKET_CHUNK_SIZE_TYPE PACKET_SIZE = 217;

// Node names for the test transferclass's
const string node1_name = "filetestnode1";
const string node2_name = "filetestnode2";
const string tname3 = "filetestnode3";
const int node1_id = 1;
const int node2_id = 8;

// CRC class for calculating crcs
CRC16 calc_crc;

// Whether to remove the created test directories or not
bool remove_test_dirs = true;

// Non-debug logging
Log::Logger test_log;
int32_t test_count = 0;
int32_t err_count = 0;
ElapsedTime test_timer;

// Debug logging to a file for more precise info
Log::Logger debug_log;
ElapsedTime tet, dt;

// Log node output to a file
Log::Logger node1_log, node2_log;

// nodeids.ini file path, and path for its backup
string nodeids_ini_path, nodeids_ini_backup_path;

// RNG seed
uint32_t seed = 0;

// Helper functions
void load_temp_nodeids();
void restore_original_nodeids();
void cleanup();
int32_t create_file(int32_t kib, string file_path);
int32_t write_bad_meta(tx_progress& tx);
int32_t write_bad_meta(tx_progress& tx, uint16_t num_bytes);
void debug_packet(PacketComm packet, uint8_t direction, string type, Log::Logger* debug_log);
template <typename T> T sumv(vector<T> vec);

// Tests
typedef int32_t (*test_func)();
void run_test(test_func, string test_name);
int32_t test_zero_size_files();
int32_t test_large_files();
int32_t test_stop_resume();
int32_t test_stop_resume2();
int32_t test_packet_reqcomplete();
int32_t test_many_files();
int32_t test_packet_cancel_missed();
int32_t test_bad_meta();
int32_t test_chaotic_order();

// Used in bad_meta test
struct old_metalong
{
    char node_name[COSMOS_MAX_NAME+1];
    PACKET_TX_ID_TYPE tx_id;
    char agent_name[COSMOS_MAX_NAME+1];
    char file_name[TRANSFER_MAX_FILENAME];
    PACKET_FILE_SIZE_TYPE file_size;
};

// Hold common test parameters to reuse for testing and verification steps
struct test_params
{
    // Number of files to create
    size_t num_files = 3;
    // Name of folder under node_name/outgoing/ and node_name/incoming/
    string agent_subfolder_name;
    // Check crcs before and after sending
    map<string, uint16_t> file_crcs;
    // Size of the created files
    map<string, int32_t> file_sizes;
    // Create files for node 1 to send to node 2
    string orig_out_dir;
    string dest_in_dir;

    // orig_node: name of the origin node
    // dest_node: name of the destination node
    // file_size_kib: size of the created files (in kib)
    // num_files: number of files to create
    // agent_subfolder_name: name of the folder to create inside outgoing and incoming folders. Use the name of the test.
    int32_t init(string orig_node, string dest_node, double file_size_kib, size_t num_files, string agent_subfolder_name)
    {
        return init(orig_node, dest_node, file_size_kib, num_files, agent_subfolder_name, "");
    }

    // orig_node: name of the origin node
    // dest_node: name of the destination node
    // file_size_kib: size of the created files (in kib)
    // num_files: number of files to create
    // agent_subfolder_name: name of the folder to create inside outgoing and incoming folders. Use the name of the test.
    // suffix: any additional string to append to the filename
    int32_t init(string orig_node, string dest_node, double file_size_kib, size_t num_files, string agent_subfolder_name, string suffix)
    {
        this->num_files = num_files;
        this->agent_subfolder_name = agent_subfolder_name;
        int32_t file_size_bytes = file_size_kib * 1024;
        orig_out_dir = data_base_path(dest_node, "outgoing", agent_subfolder_name);
        dest_in_dir = data_base_path(orig_node, "incoming", agent_subfolder_name);
        int32_t iretn = 0;
        for (size_t i = 0; i < this->num_files; ++i)
        {
            string tfilename = "file_" + std::to_string(i) + suffix;
            iretn = create_file(file_size_kib, orig_out_dir + "/" + tfilename);
            if (iretn < 0)
            {
                return iretn;
            }
            file_crcs[tfilename] = calc_crc.calc_file(orig_out_dir + "/" + tfilename);
            file_sizes[tfilename] = file_size_bytes;
        }
        return 0;
    }

    // orig_node: name of the origin node
    // dest_node: name of the destination node
    // file_size_kib_lo: lower bound of size of created files (in kib)
    // file_size_kib_hi: upper bound of size of created files (in kib)
    // num_files: number of files to create. File sizes will be distributed linearly.
    // agent_subfolder_name: name of the folder to create inside outgoing and incoming folders. Use the name of the test.
    int32_t init(string orig_node, string dest_node, int32_t file_size_kib_lo, int32_t file_size_kib_hi, size_t num_files, string agent_subfolder_name)
    {
        return init(orig_node, dest_node, file_size_kib_lo, file_size_kib_hi, num_files, agent_subfolder_name, "");
    }

    // orig_node: name of the origin node
    // dest_node: name of the destination node
    // file_size_kib_lo: lower bound of size of created files (in kib)
    // file_size_kib_hi: upper bound of size of created files (in kib)
    // num_files: number of files to create. File sizes will be distributed linearly.
    // agent_subfolder_name: name of the folder to create inside outgoing and incoming folders. Use the name of the test.
    // suffix: any additional string to append to the filename
    int32_t init(string orig_node, string dest_node, int32_t file_size_kib_lo, int32_t file_size_kib_hi, size_t num_files, string agent_subfolder_name, string suffix)
    {
        this->num_files = num_files;
        this->agent_subfolder_name = agent_subfolder_name;
        orig_out_dir = data_base_path(dest_node, "outgoing", agent_subfolder_name);
        dest_in_dir = data_base_path(orig_node, "incoming", agent_subfolder_name);
        double m;
        if (num_files == 1)
        {
            m = 0;
            file_size_kib_lo = file_size_kib_hi;
        }
        else
        {
            m = abs(file_size_kib_hi - file_size_kib_lo) / double(num_files-1);
        }
        int32_t iretn = 0;
        for (size_t i = 0; i < num_files; ++i)
        {
            int32_t file_size_kib = m * i + file_size_kib_lo;
            int32_t file_size_bytes = file_size_kib * 1024;
            string tfilename = "file_" + std::to_string(i);
            iretn = create_file(file_size_kib, orig_out_dir + "/" + tfilename + suffix);
            if (iretn < 0)
            {
                return iretn;
            }
            file_crcs[tfilename] = calc_crc.calc_file(orig_out_dir + "/" + tfilename);
            file_sizes[tfilename] = file_size_bytes;
        }
        return 0;
    }

    // Verify that any files that were transferred are identical to the originals that were being sent
    // orig_node_name: Name of the origin node
    // expected_file_num: Number of files you expect to see in the incoming folder
    int32_t verify_incoming_dir(string orig_node_name, size_t expected_file_num)
    {
        int32_t iretn = 0;
        vector<filestruc> incoming_dir = data_list_files(orig_node_name, "incoming", agent_subfolder_name);
        if (incoming_dir.size() != expected_file_num)
        {
            debug_log.Printf("Verification fail: File count incorrect. incoming_dir: %d, expected: %d\n", incoming_dir.size(), expected_file_num);
            --iretn;
        }
        for (filestruc& file : incoming_dir)
        {
            if (file_crcs.find(file.name) == file_crcs.end() || file_sizes.find(file.name) == file_sizes.end())
            {
                debug_log.Printf("Verification fail: File name error. %s %d\n", file.name.c_str(), file.size);
                --iretn;
                continue;
            }
            uint16_t crc_recv = calc_crc.calc_file(dest_in_dir + "/" + file.name);
            if (file_crcs[file.name] != crc_recv)
            {
                debug_log.Printf("Verification fail: CRC mismatch. %s %d:%d\n", file.name.c_str(), file_crcs[file.name], crc_recv);
                --iretn;
            }
            if (file.size != file_sizes[file.name])
            {
                debug_log.Printf("Verification fail: File size error. %s %d:%d\n", file.name.c_str(), file.size, file_sizes[file.name]);
                --iretn;
            }
        }
        return iretn;
    }

    // Verify that there are the number of files in the outgoing directory that you expect
    // dest_node_name: Name of the destination node
    // expected_file_num: Number of files you expect to see in the outgoing folder
    int32_t verify_outgoing_dir(string dest_node_name, size_t expected_file_num)
    {
        int32_t iretn = 0;
        vector<filestruc> outgoing_dir= data_list_files(dest_node_name, "outgoing", agent_subfolder_name);
        if (outgoing_dir.size() != expected_file_num)
        {
            debug_log.Printf("Verification fail: File count incorrect. outgoing_dir: %d, expected: %d\n", outgoing_dir.size(), expected_file_num);
            --iretn;
        }
        return iretn;
    }

    // Verify that the temp directory containing the meta files are what is expected
    // orig_node_name: Name of the origin node
    // expected_file_num: Number of files you expect to see in the incoming folder
    int32_t verify_temp_dir(string orig_node_name, size_t expected_file_num)
    {
        int32_t iretn = 0;
        vector<filestruc> temp_dir= data_list_files(orig_node_name, "temp", "file");
        if (temp_dir.size() != expected_file_num)
        {
            debug_log.Printf("Verification fail: File count incorrect. %s/temp/file: %d, expected: %d\n", orig_node_name.c_str(), temp_dir.size(), expected_file_num);
            --iretn;
        }

        return iretn;
    }

    // Get total size of test files created
    size_t get_total_bytes()
    {
        size_t size = 0;
        for (auto it = file_sizes.begin(); it != file_sizes.end(); ++it)
        {
            size += it->second;
        }
        return size;
    }
};

// main loop
int main(int argc, char *argv[])
{
    // Optional argument packet_size
    if (argc > 1)
    {
        try
        {
            remove_test_dirs = !!std::stoi(argv[1]);
        }
        catch (std::exception const &e)
        {
            std::cout << "Error: <packet_size> argument was invalid: " << e.what() << std::endl;
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    if (argc > 2)
    {
        try
        {
            PACKET_SIZE = std::stoi(argv[2]);
        }
        catch (std::exception const &e)
        {
            std::cout << "Error: <packet_size> argument was invalid: " << e.what() << std::endl;
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }

    // Setup log paths and settings
    string debug_log_path = get_cosmosnodes() + "file_transfer_tests";
    string node1_log_path = get_cosmosnodes() + "node1_transfer_test_log";
    string node2_log_path = get_cosmosnodes() + "node2_transfer_test_log";
    test_log.Set(Log::LogType::LOG_STDOUT_FFLUSH, true, "", 1800., "");
    debug_log.Set(Log::LogType::LOG_FILE_FFLUSH, false, debug_log_path, 1., "");
    node1_log.Set(Log::LogType::LOG_FILE_FFLUSH, false, node1_log_path, 1., "");
    node2_log.Set(Log::LogType::LOG_FILE_FFLUSH, false, node2_log_path, 1., "");
    test_log.Printf("Log files created at:\n%s\n%s\n%s\n", debug_log_path.c_str(), node1_log_path.c_str(), node2_log_path.c_str());

    // Randomize seed
    seed = decisec();
    srand(seed);
    debug_log.Printf("%5d | Using rand seed:%d\n", __LINE__, seed);

    //////////////////////////////////////////////////////////////////////////
    // Run tests
    //////////////////////////////////////////////////////////////////////////
    run_test(test_zero_size_files, "test_zero_size_files");
    run_test(test_large_files, "test_large_files");
    run_test(test_stop_resume, "test_stop_resume");
    run_test(test_stop_resume2, "test_stop_resume2");
    run_test(test_packet_reqcomplete, "test_packet_reqcomplete");
    run_test(test_many_files, "test_many_files"); // This one takes about 16 seconds, comment out to save some time to test other tests
    run_test(test_packet_cancel_missed, "test_packet_cancel_missed");
    run_test(test_bad_meta, "test_bad_meta");
    run_test(test_chaotic_order, "test_chaotic_order"); // This test is a bit janky, think of it more as an investigative test. Doesn't necessarily have to pass.

    //////////////////////////////////////////////////////////////////////////
    // Clean up
    //////////////////////////////////////////////////////////////////////////

    debug_log.Printf("%s\n", "Cleaning up.");
    // cleanup();


    //////////////////////////////////////////////////////////////////////////
    // Exit
    //////////////////////////////////////////////////////////////////////////

    // Final verdict
    
    test_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
    debug_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
    node1_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
    node2_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
    test_log.Printf("Exiting...\n");
    debug_log.Printf("Exiting...\n\n");
    node1_log.Printf("Exiting...\n\n");
    node2_log.Printf("Exiting...\n\n");

    test_log.Close();
    debug_log.Close();
    node1_log.Close();
    node2_log.Close();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Tests
//////////////////////////////////////////////////////////////////////////////

//! Run a test function
//! \param test A function pointer to an int32_t with no params
//! \param test_name What to log as the name of this test
//! \return n/a
void run_test(test_func test, string test_name)
{
    int32_t iretn = 0;
    ++test_count;
    test_log.Printf("%-38s", ("Running " + test_name + "...").c_str());
    debug_log.Printf("%s\n", ("===== Running " + test_name + " =====").c_str());
    node1_log.Printf("%s\n", ("===== Running " + test_name + " =====").c_str());
    node2_log.Printf("%s\n", ("===== Running " + test_name + " =====").c_str());
    test_timer.lap();
    iretn = test();
    if (iretn < 0)
    {
        test_log.Printf("...encountered error.\n");
        debug_log.Printf("%5s | Test %s encountered error.\n", "FAIL", test_name.c_str());
        ++err_count;
    }
    else
    {
        test_log.Printf("...success.\n");
        debug_log.Printf("%5s | Test %s passed.\n", "OK", test_name.c_str());
    }
    node1_log.Printf("\n");
    node2_log.Printf("\n");

    double test_runtime = test_timer.lap();
    debug_log.Printf("Test elapsed time: %4.2f seconds\n", test_runtime);

    if (remove_test_dirs)
    {
        cleanup();
    }
    debug_log.Printf("\n");
}

// Node 1 attempts to transfer zero-size files to node 2
// Expect: Nothing to transfer, but zero-size files remain in outgoing (reconsider?)
int32_t test_zero_size_files()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    const size_t num_files = 3;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, 0., num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    iretn = node1.Init(node1_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    node2.set_packet_size(PACKET_SIZE);

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Start transfer process
    iretn = node1.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    int32_t runs = 0;
    int32_t runlimit = 0;
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2.receive_packet(lpacket);
            if (iretn == node2.RESPONSE_REQUIRED)
            {
                respond = true;
                
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, runs: %d, runlimit: %d\n", __LINE__, lpackets.size(), rs.c_str(), runs, runlimit);
            break;
        }
        if (runs > runlimit)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, runs: %d, runlimit: %d\n", __LINE__, lpackets.size(), rs.c_str(), runs, runlimit);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1.receive_packet(rpacket);
            }
            respond = false;
        }

        // break if runlimit is reached
        if (runs > runlimit)
        {
            break;
        }

        ++runs;
    }
endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (runs > runlimit)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. Runs: %d, runlimit: %d\n", runs, runlimit);
        --iretn;
    }

    // Zero-size files were ignored
    iretn += test.verify_incoming_dir(node1_name, 0);
    iretn += test.verify_outgoing_dir(node2_name, num_files);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);
    

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 transfers multiple large files to Node 2, no other tricks
// Expect: Stuff to transfer
int32_t test_large_files()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 3;
    double file_size_kib = 2.;
    double file_size_bytes = file_size_kib * 1024;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    iretn = node1.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    node2.set_packet_size(PACKET_SIZE);

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Start transfer process
    iretn = node1.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    const int32_t packet_data_size_limit = node1.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes / packet_data_size_limit)   // number of DATA packets
        + num_files     // number of METADATA packets
        + 1             // number of QUEUE packets
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2.receive_packet(lpacket);
            if (iretn == node2.RESPONSE_REQUIRED)
            {
                respond = true;
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if (sumv(packets_sent) > packet_expected_total)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                ++packets_sent[NODE2];
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1.receive_packet(rpacket);
            }
            respond = false;
        }

        // break if estimate is exceeded
        if (sumv(packets_sent) > packet_expected_total)
        {
            break;
        }
    }
endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (sumv(packets_sent) > packet_expected_total)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        --iretn;
    }

    // File was successfully transferred
    iretn += test.verify_incoming_dir(node1_name, num_files);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 starts transferring stuff to Node 2, node 1 stops, then reloads and resumes again
// Note: in the current configuration, sends all data before restarting. When runlimit_init is lower, all data is sent through twice.
// Expect: File transfer picks up where it left off
int32_t test_stop_resume()
{
    int32_t iretn = 0;
    // First load, then stop
    Transfer node1a, node2a;
    // Second load after stop
    Transfer node1b;
    size_t num_files = 50;
    double file_size_kib = 6.;
    double file_size_bytes = file_size_kib * 1024;
    
    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    iretn = node1a.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2a.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return iretn;
    }
    iretn = node1a.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    node2a.set_packet_size(PACKET_SIZE);

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Start transfer process
    iretn = node1a.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    // Restart at halfway point
    const int32_t packet_data_size_limit = node1a.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    int32_t restart_run = ceil(file_size_bytes / packet_data_size_limit)/2;
    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes / packet_data_size_limit)   // number of DATA packets
        + num_files*2   // number of METADATA packets, twice since node1 restarts
        + 1*2           // number of QUEUE packets, twice since node1 restarts
        + num_files     // number of REQDATA packets (sent by default)
        + 0             // number of REQCOMPLETE packets
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets

    // Perform first run to all-data-sent/write_meta point, then stop
    for (int runs=0; runs < restart_run; ++runs)
    {
        if (runs == restart_run-1)
        {
            // write_meta only writes metadata every 5 seconds by default, so allow time for last iteration to be written to disk
            secondsleep(5.);
        }
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2a.receive_packet(lpacket);
            if (iretn == node2a.RESPONSE_REQUIRED)
            {
                respond = true;
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if (sumv(packets_sent) > packet_expected_total)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            // Don't respond in first run
        }

        // break if estimate is exceeded
        if (sumv(packets_sent) > packet_expected_total)
        {
            break;
        }
    }

    node1_log.Printf("------------------------\n--- Restarting node1 ---\n------------------------\n");
    node2_log.Printf("------------------------\n--- Restarting node1 ---\n------------------------\n");
    node1_log.Printf("(Packets sent so far: node1:%d node2:%d)\n", packets_sent[NODE1], packets_sent[NODE2]);
    node2_log.Printf("(Packets sent so far: node1:%d node2:%d)\n", packets_sent[NODE1], packets_sent[NODE2]);
    // Now start up node1b and resume file transfer
    // Load test nodeid table
    load_temp_nodeids();
    iretn = node1b.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        // Restore old nodeids.ini file here in case test crashes
        restore_original_nodeids();
        return -1;
    }
    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    node1b.set_packet_size(PACKET_SIZE);
    // Note: with enabled-by-default, this is unnecessary
    // iretn = node1b.outgoing_tx_load(node2_name);
    // if (iretn < 0)
    // {
    //     debug_log.Printf("Error in outgoing_tx_load\n");
    //     return iretn;
    // }

    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1b.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2a.receive_packet(lpacket);
            if (iretn == node2a.RESPONSE_REQUIRED)
            {
                respond = true;
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if (sumv(packets_sent) > packet_expected_total)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            node2a.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                ++packets_sent[NODE2];
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1b.receive_packet(rpacket);
            }
            respond = false;
        }

        // break if runlimit is reached
        if (sumv(packets_sent) > packet_expected_total)
        {
            break;
        }
    }
endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (sumv(packets_sent) > packet_expected_total)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        --iretn;
    }

    // File was successfully transferred
    iretn += test.verify_incoming_dir(node1_name, num_files);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1b.outgoing_tx_recount(node2_name) || node2a.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue check fail. node1b outgoing: %d, node2a incoming: %d\n", node1b.outgoing_tx_recount(node2_name), node2a.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 starts transferring stuff to Node 2, node 2 stops and misses a few packets, then restarts and resumes again
// Expect: Node 2 to request the couple of packets that it missed after restarting
int32_t test_stop_resume2()
{
    int32_t iretn = 0;
    // First load, then stop
    Transfer node1a, node2a;
    // Second load after stop
    Transfer node2b;
    size_t num_files = 50;
    double waittime_sec = 0.5;
    double file_size_kib = ((PACKET_SIZE/1024)+1)*10;
    double file_size_bytes = file_size_kib * 1024;
    // Attempt one sleep to retrigger response wait cycle
    bool waitted_once = false;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    iretn = node1a.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return -1;
    }
    iretn = node2a.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return -1;
    }
    iretn = node1a.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    iretn = node2a.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();


    // Modify wait times to not have to wait forever
    iretn = node1a.set_waittime(node2_name, 2, waittime_sec);
    iretn = node2a.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
        return iretn;
    }
    // Allow trigger of first response
    secondsleep(waittime_sec);

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Start transfer process
    iretn = node1a.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Number of runs to miss
    int miss = 2;
    // We expect an additional n+2 runs to complete from node restart, where n is however many packets for node2a to miss.
    // 1) node1 REQCOMPLETE send, node2b REQDATA send
    // n) node1 DATA send (for however many packets were missed), node2b COMPLETE send
    // n+1) node1 CANCEL send
    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    // Restart at halfway point
    const int32_t packet_data_size_limit = node1a.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    int32_t restart_run = ceil(file_size_bytes / packet_data_size_limit)/2;
    if (restart_run-miss-1 <= 0)
    {
        debug_log.Printf("Error, restart_run must be greater than 0 or the results will not be accurate. Adjust packet_size, file_size_kb, or miss\n");
        return GENERAL_ERROR_ERROR;
    }
    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes / packet_data_size_limit)   // number of DATA packets assuming no drops
        + (num_files * miss)    // additional number of DATA packets that are requested because they were missed
        + (num_files*2)         // number of METADATA packets, twice since node2 restarts
        + 2                     // number of QUEUE packets, twice since node2 restarts
        + 1                     // number of REQMETA packets
        + num_files             // number of REQCOMPLETE packets
        + num_files*2           // number of REQDATA packets (gets sent out immediately once)
        + num_files             // number of COMPLETE packets
        + num_files;            // number of CANCEL packets
    // Perform first run to all-data-sent/write_meta point, then stop
    for (int runs=0; runs < restart_run; ++runs)
    {
        if (runs == restart_run-miss-1)
        {
            // write_meta only writes metadata every 5 seconds by default, so allow time for last iteration to be written to disk
            secondsleep(5.);
        }
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            // Have node 2 receive all these packets, but skip a few
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            if (runs < restart_run - miss)
            {
                debug_packet(lpacket, 0, "Incoming", &node2_log);
                iretn = node2a.receive_packet(lpacket);
                if (iretn == node2a.RESPONSE_REQUIRED)
                {
                    respond = true;
                }
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if (sumv(packets_sent) > packet_expected_total)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            // Don't respond in first run
        }

        // break if runlimit is reached
        if (sumv(packets_sent) > packet_expected_total)
        {
            break;
        }
    }

    node1_log.Printf("------------------------\n--- Restarting node2 ---\n------------------------\n");
    node2_log.Printf("------------------------\n--- Restarting node2 ---\n------------------------\n");
    node1_log.Printf("(Packets sent so far: node1:%d node2:%d)\n", packets_sent[NODE1], packets_sent[NODE2]);
    node2_log.Printf("(Packets sent so far: node1:%d node2:%d)\n", packets_sent[NODE1], packets_sent[NODE2]);
    // Now start up node2b and resume file transfer
    // Load test nodeid table
    load_temp_nodeids();
    iretn = node2b.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        // Restore old nodeids.ini file here in case test crashes
        restore_original_nodeids();
        return -1;
    }
    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();
    iretn = node2b.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }

    iretn = node2b.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
        return iretn;
    }
    secondsleep(waittime_sec);

    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2b.receive_packet(lpacket);
            if (iretn == node2b.RESPONSE_REQUIRED)
            {
                respond = true;
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            if (!waitted_once)
            {
                waitted_once = true;
                secondsleep(waittime_sec);
                continue;
            }
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if (sumv(packets_sent) > packet_expected_total)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            node2b.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                ++packets_sent[NODE2];
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1a.receive_packet(rpacket);
            }
            respond = false;
        }

        // break if runlimit is reached
        if (sumv(packets_sent) > packet_expected_total)
        {
            break;
        }
    }
endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (sumv(packets_sent) > packet_expected_total)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        --iretn;
    }

    // File was successfully transferred
    iretn += test.verify_incoming_dir(node1_name, num_files);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1a.outgoing_tx_recount(node2_name) || node2b.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue check fail. node1a outgoing: %d, node2b incoming: %d\n", node1a.outgoing_tx_recount(node2_name), node2b.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 2's first received packet of a transfer is REQCOMPLETE
// Expect: Node 2 to receive the REQCOMPLETE packet, then communicate for missing data
int32_t test_packet_reqcomplete()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 1;
    double waittime_sec = 3.;
    double file_size_kib = 2.;
    double file_size_bytes = file_size_kib * 1024;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    iretn = node1.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    node2.set_packet_size(PACKET_SIZE);

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Modify wait times to not have to wait forever
    iretn = node1.set_waittime(node2_name, 2, waittime_sec);
    iretn = node2.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
    }

    // Start transfer process
    iretn = node1.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    const int32_t packet_data_size_limit = node1.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    // Up to last DATA packet, node2 starts listening after this many runs have passed
    int32_t runs = 0;
    // +2 for the two REQCOMPLETE packets, then +1 run for the waittime wait, then the +1 at the end for the CANCEL packet
    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes / packet_data_size_limit)*2   // number of DATA packets, everything gets sent twice
        + (num_files*2) // number of METADATA packets
        + 2             // number of QUEUE packets
        + (num_files*2) // number of REQCOMPLETE packets, gets sent twice
        + 1             // number of REQMETA packets
        + num_files*2   // number of REQDATA packets (gets sent out immediately once)
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets

    // Wait only once for the second REQCOMPLETE
    bool waited = false;

    // Sleep so that the first REQCOMPLETE can be sent
    secondsleep(waittime_sec+.5);
    // Start receiving after REQCOMPLETE is received
    bool start_receiving = false;
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            if (lpacket.header.type == PacketComm::TypeId::DataFileReqComplete)
            {
                start_receiving = true;
            }
            // Have node 2 start receiving only from and after the REQCOMPLETE packet
            if (start_receiving)
            {
                debug_packet(lpacket, 0, "Incoming", &node2_log);
                iretn = node2.receive_packet(lpacket);
                if (iretn == node2.RESPONSE_REQUIRED)
                {
                    respond = true;
                }
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            // Wait once for second REQCOMPLETE
            if (!waited)
            {
                waited = true;
                sleep(waittime_sec+0.5);
                continue;
            }
            else
            {
                string rs = respond ? "true" : "false";
                debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
                break;
            }
        }
        if (sumv(packets_sent) > packet_expected_total*2)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                ++packets_sent[NODE2];
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1.receive_packet(rpacket);
            }
            respond = false;
        }

        // break if expected total exceeded by a large amount
        if (sumv(packets_sent) > packet_expected_total*2)
        {
            break;
        }
        ++runs;
    }
endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (sumv(packets_sent) > packet_expected_total)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        --iretn;
    }

    // File was successfully transferred
    iretn += test.verify_incoming_dir(node1_name, num_files);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 has more than 256 files to send over
// Expect: txq will, because of the queueing algorithm, rather than completely fill up, fill up only partly.
//          Upon sending files completely over, spaces will open up for more to send over.
int32_t test_many_files()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 300;
    double waittime_sec = 1.;
    int32_t file_size_kib_lo = 1;
    int32_t file_size_kib_hi = 50;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib_lo, file_size_kib_hi, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    iretn = node1.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    node2.set_packet_size(PACKET_SIZE);

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    bool load_more = false;

    // Start transfer process
    iretn = node1.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }

    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    const int32_t packet_data_size_limit = node1.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    // A bit difficult to estimate, but this should be the upper bound
    int32_t packet_expected_total
        = ceil(test.get_total_bytes() / double(packet_data_size_limit))   // number of DATA packets
        + (num_files*2) // number of METADATA packets
        + 1*num_files   // number of QUEUE packets, will be much less than this
        + num_files     // number of REQCOMPLETE packets
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets

    // Modify wait times to not have to wait forever
    iretn = node1.set_waittime(node2_name, 1, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
    }
    while (true)
    {

        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2.receive_packet(lpacket);
            if (iretn == node2.RESPONSE_REQUIRED)
            {
                respond = true;
            }
        }
        if (load_more)
        {
            // Presumably, something has finished
            // Load more files as more space opens up
            iretn = node1.outgoing_tx_load(node2_name);
            if (iretn < 0)
            {
                debug_log.Printf("Error in outgoing_tx_load\n");
                return iretn;
            }
            load_more = false;
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if (sumv(packets_sent) > packet_expected_total)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                ++packets_sent[NODE2];
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1.receive_packet(rpacket);
            }
            respond = false;
            load_more = true;
        }

        // break if runlimit is reached
        if (sumv(packets_sent) > packet_expected_total)
        {
            break;
        }
    }
endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (sumv(packets_sent) > packet_expected_total)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        --iretn;
    }

    // File was successfully transferred
    iretn += test.verify_incoming_dir(node1_name, num_files);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 sends file and Node 2 receives completely up until Node 1's CANCEL packets, which are missed.
// Additionally, the first file of the bunch has its COMPLETE packet missed too.
// Node 1 then queues up some more files to send. The txid for the one that missed the COMPLETE packet stays, but
// the txids that were previously used by the ones which sent out the CANCEL (but were missed by Node 2) are reused
// for new transactions.
// When Node 1 starts sending out packets again, Node 1 sends REQCOMPLETE for the first, then normally for the new files.
// Node 2 sends a COMPLETE, and then overwrites the txid for its internal receive list for the txids that missed the CANCEL
// packets, since they were already completed and moved.
// Files are then transferred normally.
// Expect: Node 2 receives all files (none are overwritten or omitted)
int32_t test_packet_cancel_missed()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 3;
    double waittime_sec = 1.;
    double file_size_kib = 2.;
    double file_size_bytes = file_size_kib * 1024;

    // Initialize test parameters
    test_params test;
    // Create first set of files, these will have CANCEL packets missed
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    // Initialize file transfer classes
    iretn = node1.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    iretn = node2.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }
    // Modify wait times to not have to wait forever
    iretn = node1.set_waittime(node2_name, 1, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
    }

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Start transfer process
    // Load first set of files
    iretn = node1.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    const int32_t packet_data_size_limit = node1.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    // How many packets are expected to be sent out, the sum of the number of packets sent out by both nodes
    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes / packet_data_size_limit)*2   // number of DATA packets, everything gets sent twice
        + num_files*2     // number of METADATA packets
        + 1*2             // number of QUEUE packets
        + num_files*2     // number of REQCOMPLETE packets, gets sent twice
        + num_files*2     // number of COMPLETE packets, gets sent twice
        + num_files*2;    // number of CANCEL packets, gets sent twice (though only received once)

    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            // Have node 2 miss the CANCEL packet this first iteration
            if (lpacket.header.type != PacketComm::TypeId::DataFileCancel)
            {
                debug_packet(lpacket, 0, "Incoming", &node2_log);
                iretn = node2.receive_packet(lpacket);
                if (iretn == node2.RESPONSE_REQUIRED)
                {
                    respond = true;
                }
            } else {
                debug_packet(lpacket, 0, "IncoMISS", &node2_log);
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if (sumv(packets_sent) > packet_expected_total*2)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (size_t i=0; i<rpackets.size(); ++i)
            {
                ++packets_sent[NODE2];
                debug_packet(rpackets[i], 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpackets[i].data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpackets[i].header.type, rpackets[i].data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                // Don't send a COMPLETE packet on the first one of the bunch
                // in order to verify proper handling of an incomplete
                // transaction being "overwritten"
                bool skip_complete = (i == 0);
                if (rpackets[i].header.type != PacketComm::TypeId::DataFileComplete || !skip_complete)
                {
                    debug_packet(rpackets[i], 0, "Incoming", &node1_log);
                    node1.receive_packet(rpackets[i]);
                } else {
                    debug_packet(rpackets[i], 0, "IncoMISS", &node1_log);
                }
            }
            respond = false;
        }

        // break if expected total exceeded by a large amount
        if (sumv(packets_sent) > packet_expected_total*2)
        {
            break;
        }
    } // End while

    // Create more test files on sender side
    // With current library, txid will be identical if file size is identical,
    // causing a collision.
    debug_log.Printf("%5d | Node 1 creating more files...\n", __LINE__, lpackets.size());
    node1_log.Printf("------------------------\n--- Node 1 creating more files ---\n------------------------\n");
    node2_log.Printf("------------------------\n--- Node 1 creating more files ---\n------------------------\n");
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__, "_iter2");
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }
    // Load second set of files
    iretn = node1.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Sleep a bit to retrigger REQCOMPLETE sending
    sleep(waittime_sec+0.5);

    // Run again the full time through
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2.receive_packet(lpacket);
            if (iretn == node2.RESPONSE_REQUIRED)
            {
                respond = true;
            }
        }

        // break if transfers stop
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
            break;
        }
        if ((sumv(packets_sent) > packet_expected_total*2))
        {
            respond = true;
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                ++packets_sent[NODE2];
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1.receive_packet(rpacket);
            }
            respond = false;
        }

        // break if expected total exceeded by a large amount
        if (sumv(packets_sent) > packet_expected_total*2)
        {
            break;
        }
    } // End while
endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (sumv(packets_sent) > packet_expected_total)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        --iretn;
    }

    // File was successfully transferred
    iretn += test.verify_incoming_dir(node1_name, num_files*2);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Due to either file corruption or using a previous version of transferclass, the .meta file
// is badly formed.
// Expect: Bad meta files are deleted
int32_t test_bad_meta()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 0;
    double file_size_kib = 0.;

    // Initialize test parameters
    test_params test;
    // Create first set of files
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Create bad meta files
    size_t num_old_meta_files = 50;
    for (size_t i=0; i < num_old_meta_files; ++i)
    {
        tx_progress tx;
        tx.temppath = data_base_path(node1_name, "temp", "file", "out_"+std::to_string(i));
        tx.agent_name = __func__;
        tx.node_name = node1_name;
        tx.file_size = rand() % 65535;
        iretn = write_bad_meta(tx);
        if (iretn < 0)
        {
            debug_log.Printf("Error creating bad old meta file %d\n", iretn);
            return iretn;
        }
    }
    // write bad gibberish meta data
    size_t num_gibberish_meta_files = 50;
    for (size_t i=num_old_meta_files; i < num_old_meta_files+num_gibberish_meta_files; ++i)
    {
        tx_progress tx;
        tx.temppath = data_base_path(node2_name, "temp", "file", "in_"+std::to_string(i));
        tx.agent_name = __func__;
        tx.node_name = node2_name;
        tx.file_size = rand() % 65535;
        iretn = write_bad_meta(tx, tx.file_size);
        if (iretn < 0)
        {
            debug_log.Printf("Error creating bad gibberish meta file %d\n", iretn);
            return iretn;
        }
    }

    // Load nodeid table
    load_temp_nodeids();

    // Initialize file transfer classes
    iretn = node1.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        restore_original_nodeids();
        return iretn;
    }
    iretn = node2.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        restore_original_nodeids();
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        restore_original_nodeids();
        return iretn;
    }
    iretn = node2.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        restore_original_nodeids();
        return iretn;
    }

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    // Verify expected results
    iretn = 0;

    // File was successfully transferred
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 sends packets in a completely chaotic, random order, with differing packet sizes, and some duplicated
// Expect: Stuff to transfer
int32_t test_chaotic_order()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 1;
    double file_size_kib = 15.;
    double file_size_bytes = file_size_kib * 1024;
    // Because of haphazard nature of this test, there's the possibility of weird meta stuff going around at the end of the test.
    // When that happens, we want node2 to clear things up by requesting the state of things one last time before failing the test.
    bool one_last_respond = true;
    double waittime_sec = 3.;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    load_temp_nodeids();

    iretn = node1.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        restore_original_nodeids();
        return iretn;
    }
    iretn = node2.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        restore_original_nodeids();
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        restore_original_nodeids();
        return iretn;
    }
    node2.set_packet_size(PACKET_SIZE);

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> queued_lpackets, lpackets, rpackets;
    bool respond = false;
    // Modify wait times to not have to wait forever
    iretn = node1.set_waittime(node2_name, 2, waittime_sec);
    iretn = node2.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
    }
    // Start transfer process
    iretn = node1.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    const int32_t packet_data_size_limit = node1.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    // Hard to calculate, in a perfect transfer (no chaos), shouldn't be more than this,
    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes / packet_data_size_limit)*2   // number of DATA packets, REQDATA will probably cause at least one resend because of the screwed up order
        + num_files     // number of METADATA packets
        + 1             // number of QUEUE packets
        + num_files     // number of REQDATA packets (likely to be sent out)
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets
    // but increase by a bit since repeated packets are sent and what not.
    packet_expected_total *= 3;
    
    // Allow a deviation of packet size of +/-25%
    const size_t packet_size_deviation = PACKET_SIZE * 0.25;

    // First get all packets to send out
    queued_lpackets.clear();
    size_t last_size = 0;
    while (true)
    {
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, queued_lpackets);

        // Break if all outgoing lpackets were grabbed
        if (queued_lpackets.size() == last_size)
        {
            break;
        }

        // Randomly increase or decrease the packet size
        if (!(rand()%3))
        {
            const int32_t sign = (rand()%2)*2 - 1;
            const float percent = (rand()%11) / 10;
            const int32_t deviation = packet_size_deviation * percent * sign;
            const size_t new_packet_size = PACKET_SIZE + deviation;
            node1.set_packet_size(new_packet_size);
        }

        // Occasionally repeat a random packet
        if (!(rand()%7))
        {
            for (size_t i=0; i<10; ++i)
            {
                const size_t insert_pos = queued_lpackets.size();
                const size_t copy_pos = rand()%insert_pos;
                queued_lpackets.resize(insert_pos + 1);
                std::copy_n(queued_lpackets.begin()+copy_pos, 1, queued_lpackets.begin() + insert_pos);
            }
        }

        last_size = queued_lpackets.size();
    }
    // Shuffle the queue
    auto rng = std::default_random_engine {seed};
    std::shuffle(std::begin(queued_lpackets), std::end(queued_lpackets), rng);
    node1_log.Printf("%5d | queued_lpackets size:%lu\n", __LINE__, queued_lpackets.size());
    node2_log.Printf("%5d | queued_lpackets size:%lu\n", __LINE__, queued_lpackets.size());
    // Offset in the queued_lpackets vector to start transfering again from
    size_t qlp_offset = 0;
    size_t i = qlp_offset;
    // don't break out of sending queued_lpackets immediately
    int32_t break_offset = 4;
    // Now send them all out
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            ++packets_sent[NODE1];
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            // Check packet size
            if (lpacket.data.size() > PACKET_SIZE+packet_size_deviation)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
            }
            debug_packet(lpacket, 0, "Incoming", &node2_log);
            iretn = node2.receive_packet(lpacket);
            if (iretn == node2.RESPONSE_REQUIRED)
            {
                respond = true;
            }
        }
        // Send some of the out-of-sequence packets to try to mess things up
        for (i=qlp_offset; i < queued_lpackets.size(); ++i)
        {
            ++packets_sent[NODE1];
            // Have node 2 receive all these packets
            debug_packet(queued_lpackets[i], 1, "Outgoing", &node1_log);
            // Check packet size, can't be greater than max deviation
            if (queued_lpackets[i].data.size() > PACKET_SIZE+packet_size_deviation)
            {
                debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, queued_lpackets[i].header.type, queued_lpackets[i].data.size(), PACKET_SIZE+packet_size_deviation);
                goto endoftest;
            }
            debug_packet(queued_lpackets[i], 0, "Incoming", &node2_log);
            iretn = node2.receive_packet(queued_lpackets[i]);
            if (iretn == node2.RESPONSE_REQUIRED)
            {
                respond = true;
            }
            // Node2 wants to respond, but keep sending it junk for a bit
            if (respond)
            {
                // Keep sending for a bit, don't break immediately
                --break_offset;
                if (break_offset < 0 || i >= (queued_lpackets.size()-1))
                {
                    // Resume sending from last offset
                    break_offset = 4;
                    break;
                }
            }
            // Or, switch back out of bad packets for a bit
            if (packets_sent[NODE1] % (queued_lpackets.size()/2) == 0)
            {
                secondsleep(waittime_sec);
                break_offset = 4;
                break;
            }
        }
        qlp_offset = i+1;

        if ((sumv(packets_sent) > packet_expected_total*2) && one_last_respond)
        {
            respond = true;
            // Trigger response
            secondsleep(waittime_sec+0.5);
            node1_log.Printf("Triggering one_last_respond\n");
            node2_log.Printf("Triggering one_last_respond\n");
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total*2);
        }
        if (sumv(packets_sent) > packet_expected_total*3)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total*3);
        }
        // break if transfers stop
        if (!lpackets.size() && (qlp_offset >= queued_lpackets.size()) && !respond)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%5d | lpackets.size(): %d, respond: %s, node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", __LINE__, lpackets.size(), rs.c_str(), packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total*3);
            if (one_last_respond)
            {
                respond = true;
                one_last_respond = false;
                // Trigger response
                secondsleep(waittime_sec);
                node1_log.Printf("Triggering one_last_respond\n");
                node2_log.Printf("Triggering one_last_respond\n");
            }
            else
            {
                break;
            }
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                ++packets_sent[NODE2];
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                // Check packet size
                if (rpacket.data.size() > PACKET_SIZE)
                {
                    debug_log.Printf("%5d | PACKET_SIZE exceeded! type:%d size:%d limit:%d\n", __LINE__, rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1.receive_packet(rpacket);
            }
            if (rpackets.size())
            {
                secondsleep(waittime_sec-0.1);
            }
            respond = false;
        }

        // break if expected total exceeded by a large amount
        if (sumv(packets_sent) > packet_expected_total*2)
        {
            if (one_last_respond)
            {
                one_last_respond = false;
                continue;
            }
        }
        // True break, test failed
        if (sumv(packets_sent) > packet_expected_total*3)
        {
            break;
        }
    }


endoftest:

    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (sumv(packets_sent) > packet_expected_total*3)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total*3);
        --iretn;
    }

    // File was successfully transferred
    iretn += test.verify_incoming_dir(node1_name, num_files);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

//////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////

void load_temp_nodeids()
{
    // Backup existing nodeids.ini file
    nodeids_ini_path = get_cosmosnodes() + "nodeids.ini";
    nodeids_ini_backup_path = get_cosmosnodes() + "nodeids.ini.back";
    rename(nodeids_ini_path.c_str(), nodeids_ini_backup_path.c_str());

    // Load in some arbitrary node ids
    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
    temp_nodeids_ini << node1_id << " " << node1_name << "\n";
    temp_nodeids_ini << node2_id << " " << node2_name << "\n";
    temp_nodeids_ini.close();    
}

void restore_original_nodeids()
{
    // Restore backup of nodeids.ini file
    rename(nodeids_ini_backup_path.c_str(), nodeids_ini_path.c_str());
}

void cleanup()
{
    debug_log.Printf("Removing created test directories... ");
    
    // Delete created folders, don't touch this
    rmdir(get_cosmosnodes() + "" + node1_name);
    rmdir(get_cosmosnodes() + "" + node2_name);
    rmdir(get_cosmosnodes() + "" + tname3);

    debug_log.Printf("OK.\n");
}

// Create an file of kib kibibytes at the file_path
// kb: kibibytes
// Returns 0 on success, negative on error
int32_t create_file(int32_t kib, string file_path)
{
    vector<char> bytes(1024, 0);
    for (size_t i=0; i < bytes.size(); ++i)
    {
        bytes[i] = i & 0xFF;
    }
    ofstream of(file_path, std::ios::binary | std::ios::out);
    for(int i = 0; i < kib; ++i)
    {
        if (!of.write(&bytes[0], bytes.size()))
        {
            debug_log.Printf("Error creating %s\n", file_path.c_str());
            return -1;
        }
    }
    return 0;
}

void serialize_oldmetadata(PacketComm& packet, PACKET_TX_ID_TYPE tx_id, const char* file_name, PACKET_FILE_SIZE_TYPE file_size, const char* node_name, const char* agent_name)
{
    const size_t _PACKET_METALONG_OFFSET_NODE_NAME = 0;
    const size_t _PACKET_METALONG_OFFSET_TX_ID = _PACKET_METALONG_OFFSET_NODE_NAME + COSMOS_MAX_NAME;
    const size_t _PACKET_METALONG_OFFSET_AGENT_NAME = _PACKET_METALONG_OFFSET_TX_ID + sizeof(PACKET_FILE_SIZE_TYPE);
    const size_t _PACKET_METALONG_OFFSET_FILE_NAME = _PACKET_METALONG_OFFSET_AGENT_NAME + COSMOS_MAX_NAME;
    const size_t _PACKET_METALONG_OFFSET_FILE_SIZE = _PACKET_METALONG_OFFSET_FILE_NAME + TRANSFER_MAX_FILENAME;
    const size_t _PACKET_METALONG_OFFSET_TOTAL = _PACKET_METALONG_OFFSET_FILE_SIZE + sizeof(PACKET_FILE_SIZE_TYPE);
    packet.header.type = PacketComm::TypeId::DataFileMetaData;
    packet.data.resize(_PACKET_METALONG_OFFSET_TOTAL);
    memmove(&packet.data[0]+COSMOS_MAX_NAME,      &tx_id,     sizeof(PACKET_TX_ID_TYPE));
    memmove(&packet.data[0]+_PACKET_METALONG_OFFSET_FILE_NAME,  file_name,  TRANSFER_MAX_FILENAME);
    memmove(&packet.data[0]+_PACKET_METALONG_OFFSET_FILE_SIZE,  &file_size, sizeof(file_size));
    memmove(&packet.data[0]+_PACKET_METALONG_OFFSET_NODE_NAME,  node_name,  COSMOS_MAX_NAME);
    memmove(&packet.data[0]+COSMOS_MAX_NAME+1, agent_name, COSMOS_MAX_NAME);
}

// The old way of writing meta files, prior to commit 6c05a9262c0cb7e791465e8754782d813ba95417
int32_t write_bad_meta(tx_progress& tx)
{
    PacketComm packet;
    std::ofstream file_name;

    serialize_oldmetadata(packet, tx.tx_id, tx.file_name.c_str(), tx.file_size, tx.node_name.c_str(), tx.agent_name.c_str());
    file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary); // Note: truncs by default
    if(!file_name.is_open())
    {
        return (-errno);
    }
    uint16_t crc;
    CRC16 calc_crc;
    file_name.write((char *)&packet.data[0], sizeof(old_metalong));
    crc = calc_crc.calc(packet.data);
    file_name.write((char *)&crc, 2);
    for (file_progress progress_info : tx.file_info)
    {
        file_name.write((const char *)&progress_info, sizeof(progress_info));
        crc = calc_crc.calc((uint8_t *)&progress_info, sizeof(progress_info));
        file_name.write((char *)&crc, 2);
    }
    file_name.close();

    return 0;
}

// Writes a bunch of gibberish to a meta file
// tx: include some basic setup stuff to create the meta file, like the temppath
// num_bytes the number of bytes to write to the garbage meta file
int32_t write_bad_meta(tx_progress& tx, uint16_t num_bytes)
{
    PacketComm packet;
    std::ofstream file_name;
    serialize_metadata(packet, tx.tx_id, tx.file_crc, tx.file_name, tx.file_size, tx.node_name, tx.agent_name);
    file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary); // Note: truncs by default
    if(!file_name.is_open())
    {
        return (-errno);
    }
    vector<uint8_t> write_bytes(num_bytes);
    for (size_t i=0; i<write_bytes.size(); ++i)
    {
        write_bytes[i] = rand() % 256;
    }
    file_name.write((const char *)&write_bytes, write_bytes.size());
    file_name.close();
    return 0;
}

template <typename T>
T sumv(vector<T> vec)
{
    T zero = 0;
    return std::accumulate(vec.begin(), vec.end(), zero);
}

//! For printing out debug statements about incoming and outgoing packets.
//! \param packet An incoming or outgoing packet
//! \param direction PACKET_IN or PACKET_OUT
//! \param type Incoming or outgoing, used only in the print statement
//! \param err_log Pointer to Error object to log with
//! \return n/a
void debug_packet(PacketComm packet, uint8_t direction, string type, Log::Logger* debug_log)
{
    if (packet.header.type == PacketComm::TypeId::DataFileChunkData)
    {
        // For DATA-type packets, print only the first time it comes in. Comment out these lines to skip all DATA-packet logging
        // PACKET_FILE_SIZE_TYPE chunk_start;
        // memmove(&chunk_start, &packet.data[0]+PACKET_DATA_OFFSET_CHUNK_START, sizeof(chunk_start));
        // if (chunk_start != 0)
        {
            return;
        }
    }

    if (debug_log->Type())
    {
        string node_name = NodeData::lookup_node_id_name(packet.data[0]);
        uint8_t node_id = NodeData::check_node_id(packet.data[0]);

        if (direction == 0)
        {
            debug_log->Printf("%.4f %.4f RECV L %u R %u %s [%s] Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), type.c_str(), packet.data.size());
        }
        else if (direction == 1)
        {
            debug_log->Printf("%.4f %.4f SEND L %u R %u %s [%s] Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), type.c_str(), packet.data.size());
        }

        switch (packet.header.type)
        {
        case PacketComm::TypeId::DataFileMetaData:
            {
                packet_struct_metashort meta;
                deserialize_metadata(packet.data, meta);
                debug_log->Printf("[METADATA] %u %u %s ", node_id, packet.data[offsetof(packet_struct_metashort, header.tx_id)], meta.file_name.c_str());
                break;
            }
        case PacketComm::TypeId::DataFileChunkData:
            {
                debug_log->Printf("[DATA] %u %u %u %u ", node_id, packet.data[offsetof(packet_struct_data, header.tx_id)], packet.data[offsetof(packet_struct_data, chunk_start)]+256U*(packet.data[offsetof(packet_struct_data, chunk_start)+1]+256U*(packet.data[offsetof(packet_struct_data, chunk_start)+2]+256U*packet.data[offsetof(packet_struct_data, chunk_start)+3])), packet.data[offsetof(packet_struct_data, byte_count)]+256U*packet.data[offsetof(packet_struct_data, byte_count)+1]);
                break;
            }
        case PacketComm::TypeId::DataFileReqData:
            {
                debug_log->Printf("[REQDATA] ");
                for (auto& byte : packet.data)
                {
                    debug_log->Printf("%u ", unsigned(byte));
                }
                debug_log->Printf("\n");
                break;
            }
        case PacketComm::TypeId::DataFileReqComplete:
            {
                debug_log->Printf("[REQCOMPLETE] %u %u ", node_id, packet.data[offsetof(packet_struct_reqcomplete, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileComplete:
            {
                debug_log->Printf("[COMPLETE] %u %u ", node_id, packet.data[offsetof(packet_struct_complete, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileCancel:
            {
                debug_log->Printf("[CANCEL] %u %u ", node_id, packet.data[offsetof(packet_struct_cancel, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileReqMeta:
        case PacketComm::TypeId::DataFileQueue:
            {
                packet_struct_queue queue;
                deserialize_queue(packet.data, queue);
                string label = packet.header.type == PacketComm::TypeId::DataFileReqMeta ? "REQMETA" : "QUEUE";
                debug_log->Printf("[%s] %u ", label.c_str(), node_id);
                // Note: this assumes that PACKET_QUEUE_FLAGS_TYPE is a uint8_t type
                for (PACKET_QUEUE_FLAGS_TYPE i=0; i<PACKET_QUEUE_FLAGS_LIMIT; ++i)
                {
                    PACKET_QUEUE_FLAGS_TYPE flags = queue.tx_ids[i];
                    //debug_log->Printf("[%u] ", flags);
                    PACKET_TX_ID_TYPE hi = i << 3;
                    for (size_t bit = 0; bit < sizeof(PACKET_QUEUE_FLAGS_TYPE)*8; ++bit)
                    {
                        uint8_t flag = (flags >> bit) & 1;
                        if (!flag)
                        {
                            continue;
                        }
                        PACKET_TX_ID_TYPE tx_id = hi | bit;
                        debug_log->Printf("%u ", unsigned(tx_id));
                    }
                }
            }
            break;
        default:
            {
                debug_log->Printf("[OTHER] %u %s", node_id, "Non-file transfer type in packet.header.type");
            }
        }
        debug_log->Printf("\n");
    }

    return;
}


/*
test cases:
check all arrays for segfault conditions
packet destination confusion, overlapping tx_ids
tx queue filling
files being deleted mid-transfer
didn't a directory get deleted in an outgoing subfolder?
new node_ids being added, node id mismatch between nodes
test heartbeat and command stuff
resuming, skipping with missing chunks
stopping and resuming both
making receiver less tied to tx_ids (also have better unique transaction IDs in every packet)
*/
