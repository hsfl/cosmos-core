//! For testing the file transfer protocol implemented by transferclass and transferlib
//!
//! Usage: monitor_adcs [-h|--help] [-v|--verbose] [-k|--keep-dir] [-p|--packet-size <PACKETSIZE>] [-t|--test <TESTNAME>]
//! delete_test_dirs, by default true, will remove the created test directories. 0 or 1.
//! packet_size is the size of the transfer file packet. Simulate packet size restrictions of various radios.
//! Test as of this commit successful for up to packet_size=65535, didn't test above that
//! Test as of this commit successful for as lower bound packet_size=47 (a shorter node name will make this smaller by same amount)
//! Note, a slower machine might trigger more automatic responses and thus increase
//! the number of packets sent. Tests best performed on a decent computer.

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
cosmosstruc* cinfo1;
const string node1_name = "filetestnode1";
cosmosstruc* cinfo2;
const string node2_name = "filetestnode2";
const string tname3 = "filetestnode3";
const int node1_id = 1;
const int node2_id = 8;

// CRC class for calculating crcs
CRC16 calc_crc;

// Whether to remove the created test directories or not
bool remove_test_dirs = true;

// Verbose log
bool verbose_log = false;

// User specified test
string specified_test = "";

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
void make_temp_nodeids();
void restore_original_nodeids();
void cleanup();
int32_t create_file(int32_t kib, string file_path);
int32_t write_bad_meta(tx_progress& tx);
int32_t write_bad_meta(tx_progress& tx, uint16_t num_bytes);
void debug_packet(PacketComm packet, uint8_t direction, string type, Log::Logger* debug_log);
template <typename T> T sumv(vector<T> vec);
void handle_args(int argc, char *argv[]);

// Tests
typedef int32_t (*test_func)();
int32_t run_test(test_func, string test_name);
int32_t run_all_tests();
int32_t test_zero_size_files();
int32_t test_large_files_and_queue();
int32_t test_stop_resume();
int32_t test_stop_resume2();
int32_t test_packet_reqcomplete();
int32_t test_many_files();
int32_t test_packet_cancel_missed();
int32_t test_bad_meta();
int32_t test_chaotic_order();
int32_t test_file_deleted_midrun();
int32_t test_txid_overlap();
int32_t test_wrong_protocol_version();
int32_t test_wrong_protocol_version2();
int32_t test_late_meta();
int32_t test_file_crc_check_fail();

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
            iretn = calc_crc.calc_file(orig_out_dir + "/" + tfilename);
            if (iretn < 0)
            {
                return iretn;
            }
            file_crcs[tfilename] = iretn;
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
        if (!num_files)
        {
            return 0;
        }
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
        return verify_incoming_dir(orig_node_name, expected_file_num, false);
    }
    // Verify that any files that were transferred are identical to the originals that were being sent
    // orig_node_name: Name of the origin node
    // expected_file_num: Number of files you expect to see in the incoming folder
    // perform_only_count_check: If set to true, only checks if the file count is correct, but doesn't care if other validations are mismatched
    int32_t verify_incoming_dir(string orig_node_name, size_t expected_file_num, bool perform_only_count_check)
    {
        int32_t iretn = 0;
        int32_t simple_verify = iretn;
        vector<filestruc> incoming_dir = data_list_files(orig_node_name, "incoming", agent_subfolder_name);
        if (incoming_dir.size() != expected_file_num)
        {
            debug_log.Printf("Verification fail: File count incorrect. incoming_dir: %d, expected: %d\n", incoming_dir.size(), expected_file_num);
            --iretn;
        }
        simple_verify = iretn;
        for (filestruc& file : incoming_dir)
        {
            if (file_crcs.find(file.name) == file_crcs.end() || file_sizes.find(file.name) == file_sizes.end())
            {
                debug_log.Printf("Verification fail: File name error. %s %d\n", file.name.c_str(), file.size);
                --iretn;
                continue;
            }
            int32_t crc_ret = calc_crc.calc_file(dest_in_dir + "/" + file.name);
            if (crc_ret < 0)
            {
                debug_log.Printf("Error in calc_file. %s %d\n", file.name.c_str(), file_crcs[file.name]);
                --iretn;
            }
            else
            {
                uint16_t crc_recv = crc_ret;
                if (file_crcs[file.name] != crc_recv)
                {
                    debug_log.Printf("Verification fail: CRC mismatch. %s %d:%d\n", file.name.c_str(), file_crcs[file.name], crc_recv);
                    --iretn;
                }
            }
            if (file.size != file_sizes[file.name])
            {
                debug_log.Printf("Verification fail: File size error. %s %d:%d\n", file.name.c_str(), file.size, file_sizes[file.name]);
                --iretn;
            }
        }
        return perform_only_count_check ? simple_verify : iretn;
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
    cinfo1 = json_init("node1");
    cinfo2 = json_init("node2");
    handle_args(argc, argv);

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
    specified_test == "test_zero_size_files"        ? run_test(test_zero_size_files, "test_zero_size_files") :
    specified_test == "test_large_files_and_queue"  ? run_test(test_large_files_and_queue, "test_large_files_and_queue") :
    specified_test == "test_stop_resume"            ? run_test(test_stop_resume, "test_stop_resume") :
    specified_test == "test_stop_resume2"           ? run_test(test_stop_resume2, "test_stop_resume2") :
    specified_test == "test_packet_reqcomplete"     ? run_test(test_packet_reqcomplete, "test_packet_reqcomplete") :
    specified_test == "test_many_files"             ? run_test(test_many_files, "test_many_files") : // This one takes about 16 seconds, comment out to save some time to test other tests
    specified_test == "test_packet_cancel_missed"   ? run_test(test_packet_cancel_missed, "test_packet_cancel_missed") :
    specified_test == "test_bad_meta"               ? run_test(test_bad_meta, "test_bad_meta") :
    specified_test == "test_chaotic_order"          ? run_test(test_chaotic_order, "test_chaotic_order") : // This test is a bit janky, think of it more as an investigative test. Doesn't necessarily have to pass.
    specified_test == "test_file_deleted_midrun"    ? run_test(test_file_deleted_midrun, "test_file_deleted_midrun") :
    specified_test == "test_txid_overlap"           ? run_test(test_txid_overlap, "test_txid_overlap") :
    specified_test == "test_wrong_protocol_version" ? run_test(test_wrong_protocol_version, "test_wrong_protocol_version") :
    specified_test == "test_wrong_protocol_version2"? run_test(test_wrong_protocol_version2, "test_wrong_protocol_version2") :
    specified_test == "test_late_meta"              ? run_test(test_late_meta, "test_late_meta") :
    specified_test == "test_file_crc_check_fail"    ? run_test(test_file_crc_check_fail, "test_file_crc_check_fail") :
    specified_test.empty()                          ? run_all_tests() :
    printf("No test named %s, run with --help flag to display all tests.\n", specified_test.c_str());

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
int32_t run_test(test_func test, string test_name)
{
    int32_t iretn = 0;
    ++test_count;
    test_log.Printf("%-40s", ("Running " + test_name + "...").c_str());
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

    return iretn;
}

int32_t run_all_tests()
{
    run_test(test_zero_size_files, "test_zero_size_files");
    run_test(test_large_files_and_queue, "test_large_files_and_queue");
    run_test(test_stop_resume, "test_stop_resume");
    run_test(test_stop_resume2, "test_stop_resume2");
    run_test(test_packet_reqcomplete, "test_packet_reqcomplete");
    run_test(test_many_files, "test_many_files"); // This one takes about 16 seconds, comment out to save some time to test other tests
    run_test(test_packet_cancel_missed, "test_packet_cancel_missed");
    run_test(test_bad_meta, "test_bad_meta");
    run_test(test_chaotic_order, "test_chaotic_order"); // This test is a bit janky, think of it more as an investigative test. Doesn't necessarily have to pass.
    run_test(test_file_deleted_midrun, "test_file_deleted_midrun");
    run_test(test_txid_overlap, "test_txid_overlap");
    run_test(test_wrong_protocol_version, "test_wrong_protocol_version");
    run_test(test_wrong_protocol_version2, "test_wrong_protocol_version2");
    run_test(test_late_meta, "test_late_meta");
    run_test(test_file_crc_check_fail, "test_file_crc_check_fail");

    return 0;
}

// Node 1 attempts to transfer zero-size files to node 2
// Expect: Nothing to transfer, and zero-size files are deleted
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
    make_temp_nodeids();

    iretn = node1.Init(cinfo1);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(cinfo2);
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
    int32_t runlimit = 1; // One QUEUE packet sent
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

// Node 1 transfers multiple large files to Node 2, also the QUEUE packet arrives first
// Expect: Stuff to transfer
int32_t test_large_files_and_queue()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 3;
    double file_size_kib = 2000.;
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
    make_temp_nodeids();

    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
    // Modify wait times to attempt to trigger REQDATA in DATA receive
    double waittime_sec = 1;
    iretn = node1.set_waittime(node2_name, 2, waittime_sec);
    iretn = node2.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
        return iretn;
    }
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
        + 2             // number of QUEUE packets
        - num_files     // number of REQDATA packets (don't mind the minus here, calculation below goes num_files over the expected, so adjust here. Could be sent a couple times, depending on how long process takes)
        + num_files     // number of REQCOMPLETE packets
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets
    ElapsedTime et;
    et.reset();
    bool queue_to_the_front_once = true;
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Sticking this in here just because, but this will bring the QUEUE packet to the beginning, once.
            // That will test the behaviour of a QUEUE packet arriving first
            if (queue_to_the_front_once)
            {
                for (auto it = lpackets.begin(); it != lpackets.end(); ++it)
                {
                    if (it->header.type == PacketComm::TypeId::DataFileQueue)
                    {
                        std::iter_swap(it, lpackets.begin());
                        queue_to_the_front_once = false;
                        break;
                    }
                }
            }
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

        // If test goes on, add to expected packet count
        if (et.split() > waittime_sec)
        {
            packet_expected_total += num_files;     // number of REQDATA packets (could be sent a couple times, depending on how long process takes)
            et.reset();
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
    make_temp_nodeids();

    iretn = node1a.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2a.Init(cinfo2, &node2_log);
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
        + 2*2           // number of QUEUE packets, twice since node1 restarts
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
    make_temp_nodeids();
    iretn = node1b.Init(cinfo1, &node1_log);
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
    // REQCOMPLETE sent 2 times, once more after all data is sent following a REQDATA
    int16_t num_responses = 2;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    make_temp_nodeids();

    iretn = node1a.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return -1;
    }
    iretn = node2a.Init(cinfo2, &node2_log);
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
        + 3                     // number of QUEUE packets, two times for a usual run, then one final time after success
        + 1                     // number of REQMETA packets
        + num_files*2           // number of REQCOMPLETE packets, once after reboot, once more after all data is sent following a REQDATA
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
    make_temp_nodeids();
    iretn = node2b.Init(cinfo2, &node2_log);
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
            if ((num_responses--))
            {
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
    make_temp_nodeids();

    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
        + 3             // number of QUEUE packets, two times for a usual run, then one final time after success
        + (num_files*2) // number of REQCOMPLETE packets, gets sent twice
        + 1             // number of REQMETA packets
        + num_files*2   // number of REQDATA packets (gets sent out immediately once)
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets

    // Wait only once for the second REQCOMPLETE
    bool waited = false;

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
    make_temp_nodeids();

    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
        + 2*num_files   // number of QUEUE packets, will be much less than this
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
    make_temp_nodeids();

    // Initialize file transfer classes
    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
        + 2*2             // number of QUEUE packets
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
    make_temp_nodeids();

    // Initialize file transfer classes
    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        restore_original_nodeids();
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
    make_temp_nodeids();

    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        restore_original_nodeids();
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
        + 2             // number of QUEUE packets
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

// Node 1 starts transferring files to node 2. One gets deleted midrun, another gets deleted between runs (after reboot)
// Expect: Graceful failure, transactions cancelled, queues emptied and no artifacts remaining
int32_t test_file_deleted_midrun()
{
    int32_t iretn = 0;
    // First load, then stop
    Transfer node1a, node2a;
    // Second load after stop
    Transfer node1b;
    size_t num_files = 2;
    double file_size_kib = 10.;
    double file_size_bytes = file_size_kib * 1024;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    string base_path = data_base_path(node2_name, "outgoing", __func__);
    if (!base_path.size())
    {
        debug_log.Printf("%5d | Error getting base path to test file!");
        return TRANSFER_ERROR_FILENAME;
    }
    string file1_path = base_path + "/" + (test.file_crcs.begin())->first;
    string file2_path = base_path + "/" + (++test.file_crcs.begin())->first;

    // Load nodeid table
    make_temp_nodeids();

    iretn = node1a.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2a.Init(cinfo2, &node2_log);
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
    const int32_t packet_data_size_limit = node1a.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    // Restart at halfway point
    int32_t restart_run = ceil(file_size_bytes / packet_data_size_limit)/2;
    // Delete first file in middle of first run
    int32_t delete_run = restart_run/2;

    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes / packet_data_size_limit)   // number of DATA packets
        + num_files*2   // number of METADATA packets, twice since node1 restarts
        + 3             // number of QUEUE packets, two for a usual run, one more after restart and nothing to send
        + 0             // number of REQDATA packets
        + 0             // number of REQCOMPLETE packets
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets
    // Perform first run to all-data-sent/write_meta point, then stop
    for (int runs=0; runs < restart_run; ++runs)
    {
        if (runs == delete_run)
        {
            // Delete second file
            if (!base_path.size())
            {
                debug_log.Printf("%5d | Error getting base path to test file!");
                goto endoftest;
            }
            node1_log.Printf("Removing file %s\n", file2_path.c_str());
            node2_log.Printf("Removing file %s\n", file2_path.c_str());
            remove(file2_path.c_str());
        }
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
                node1a.receive_packet(rpacket);
            }
            respond = false;
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
    
    // Delete first file
    node1_log.Printf("Removing file %s\n", file1_path.c_str());
    node2_log.Printf("Removing file %s\n", file1_path.c_str());
    remove(file1_path.c_str());
    
    // Now start up node1b and resume file transfer
    // Load test nodeid table
    make_temp_nodeids();
    iretn = node1b.Init(cinfo1, &node1_log);
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

    // At this point, receiver side will have a .file and .meta sitting on
    // some tx_id that will remain silent until an lpacket is sent for that
    // same tx_id. Receiver currently doesn't think to automatically request
    // updates. Here we'll manually do a REQQUEUE.
    {
        PacketComm rpacket;
        rpacket.header.nodedest = node1_id;
        rpacket.header.nodeorig = node2_id;
        vector<PACKET_TX_ID_TYPE> treqmeta;
        treqmeta.clear();
        serialize_reqmeta(rpacket, static_cast<PACKET_NODE_ID_TYPE>(node2_id), node1_name, treqmeta);
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
    iretn += test.verify_incoming_dir(node1_name, 2, true);
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

// Similar to test_file_deleted_midrun(), where one file is deleted inbetween boots.
// But instead, also creates another new file with the same tx_id.
// What should differ is the file_crc, which will immediately cause node2 to cancel its own mismatched versions.
// Three cases: DATA is received first on overlap, METADATA is received first on overlap, or REQCOMPLETE is received first on overlap
// Expect: Overlapped tx_id files to be moved to incoming with suffixes appended to their file names, _DATATXID_ERR, _METATXID_ERR, _REQCTXID_ERR.
//          Then, new transactions are received like usual.
int32_t test_txid_overlap()
{
    int32_t iretn = 0;
    // First load, then stop
    Transfer node1a, node2a;
    // Second load after stop
    Transfer node1b;
    size_t num_files = 3;
    double file_size_kib = 10.;
    double file_size_bytes = file_size_kib * 1024;
    // node2 does a REQMETA and a REQDATA following firsting receival of REQCOMPLETE
    int16_t num_responses = 2;


    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    string base_path = data_base_path(node2_name, "outgoing", __func__);
    string temp_path = data_base_path(node2_name, "temp", "file");
    if (!base_path.size() || !temp_path.size())
    {
        debug_log.Printf("%5d | Error getting path to test files!");
        return TRANSFER_ERROR_FILENAME;
    }

    // Load nodeid table
    make_temp_nodeids();

    iretn = node1a.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2a.Init(cinfo2, &node2_log);
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

    // Modify wait times to not have to wait forever
    double waittime_sec = 2;
    iretn = node1a.set_waittime(node2_name, 2, waittime_sec);
    iretn = node2a.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
        return iretn;
    }

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;

    // Keep track of tx_ids
    // tx_id[0] -> DATA first
    // tx_id[1] -> METADATA first
    // tx_id[2] -> REQCOMPLETE first
    vector<uint8_t> tx_ids;
    // tx_id[2] to start receiving only after a REQCOMPLETE is received first
    bool start_receiving = false;

    // Start transfer process
    iretn = node1a.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }
    // Number of packets sent by each node
    vector<int32_t> packets_sent = {0,0};
    const int32_t packet_data_size_limit = node1a.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    // Restart at halfway point
    int32_t restart_run = floor(file_size_bytes / packet_data_size_limit)/2;

    int32_t packet_expected_total
        = num_files*ceil(1.5 * file_size_bytes/packet_data_size_limit)   // number of DATA packets, 1.5 as first half is interrupted, then second run goes until the end
        + (file_size_bytes/packet_data_size_limit) // tx_id[2] needs all its data packets
        + num_files*2+1 // number of METADATA packets, twice since node1 restarts, and one more following node2's REQMETA request
        + 4             // number of QUEUE packets, 1 since node1 gets cut off, then twice for a full run after reboot, then one last one after tx_id[2] completes
        + 1             // number of REQDATA packets, tx_id[2] will request DATA after first REQCOMPLETE
        + 1             // number of REQMETA packets, tx_id[2] will request METADATA after first REQCOMPLETE
        + 1             // number of REQCOMPLETE packets
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets
    // Perform first run to all-data-sent/write_meta point, then stop
    for (int runs=0; runs < restart_run; ++runs)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Keep track of tx_ids, add all to list
            if (lpacket.header.type == PacketComm::TypeId::DataFileMetaData && std::find(tx_ids.begin(), tx_ids.end(), lpacket.data[offsetof(struct file_packet_header, tx_id)]) == tx_ids.end())
            {
                tx_ids.push_back(lpacket.data[offsetof(struct packet_struct_metadata, header.tx_id)]);
            }
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
                node1a.receive_packet(rpacket);
            }
            respond = false;
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

    if (tx_ids.size() != num_files)
    {
        debug_log.Printf("Error getting tx_ids, size: %lu expected: %lu\n", tx_ids.size(), num_files);
        return GENERAL_ERROR_ERROR;
    }
    
    // Delete files
    node1_log.Printf("Clearing folder %s\n", base_path.c_str());
    node2_log.Printf("Clearing folder %s\n", base_path.c_str());
    node1_log.Printf("Clearing folder %s\n", temp_path.c_str());
    node2_log.Printf("Clearing folder %s\n", temp_path.c_str());
    rmdir_contents(temp_path);
    rmdir_contents(base_path);

    // Add new files that overlap on tx_id
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__, "_iter2");
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }
    
    // Now start up node1b and resume file transfer
    // Load test nodeid table
    make_temp_nodeids();
    iretn = node1b.Init(cinfo1, &node1_log);
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


    iretn = node1b.set_waittime(node2_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
        return iretn;
    }

    iretn = node1b.outgoing_tx_load(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error in outgoing_tx_load\n");
        return iretn;
    }

    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1b.get_outgoing_lpackets(node2_name, lpackets);
        // tx_id[0] gets overlapping on DATA first
        // Since the METADATA packets arrive sooner than the DATA, have the
        // DATA packets arrive sooner to test 2nd case of overlapping tx_ids
        for (auto it = lpackets.begin(); it != lpackets.end(); ++it)
        {
            if (lpackets.begin()->header.type != PacketComm::TypeId::DataFileMetaData
            && it->data[offsetof(struct packet_struct_metadata, header.tx_id)] != tx_ids[0])
            {
                break;
            }
            if (it->data[offsetof(struct packet_struct_metadata, header.tx_id)] != tx_ids[0])
            {
                continue;
            }
            if (it->header.type == PacketComm::TypeId::DataFileChunkData)
            {
                std::iter_swap(it, lpackets.begin());
                break;
            }
        }
        // tx_id[1] gets overlapping on METADATA first, but nothing needs to be done, as METADATA should arrive first
        // tx_id[2] getes overlapping on REQCOMPLETE first, so discard all of its outgoing packets until a REQCOMPLETE is sent
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
            // Ignore tx_id[2]'s outgoing packets until a ReqComplete is sent.
            if (!start_receiving
            && lpacket.data[offsetof(struct packet_struct_metadata, header.tx_id)] == tx_ids[2]
            && lpacket.header.type != PacketComm::TypeId::DataFileReqComplete)
            {
                continue;
            }
            // After tx_id[2]'s first ReqComplete packet, start receiving everything.
            if (lpacket.data[offsetof(struct packet_struct_metadata, header.tx_id)] == tx_ids[2]
            && lpacket.header.type == PacketComm::TypeId::DataFileReqComplete)
            {
                start_receiving = true;
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
            if ((num_responses--))
            {
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
    iretn += test.verify_incoming_dir(node1_name, num_files*2, true);
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

// Sends a some file packets first to get node2 to create a meta file.
// Then the test will modify the meta file to make the version different.
// Also, a bunch of wrong-version packets will be sent.
// Expect: No wrong-version packets will be accepted. Wrong-version meta files will be removed.
int32_t test_wrong_protocol_version()
{
    int32_t iretn = 0;
    // First load, then stop
    Transfer node1a, node2a;
    // Second load after stop
    Transfer node2b;
    size_t num_files = 3;
    double file_size_kib = 10.;
    double file_size_bytes = file_size_kib * 1024;
    // Version number to set erroneous file packets and meta files to
    uint8_t err_version_num = 255;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, file_size_kib, num_files, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Use to modify node1's meta files later
    vector<filestruc> temp_dir;

    // Load nodeid table
    make_temp_nodeids();

    iretn = node1a.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2a.Init(cinfo2, &node2_log);
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
    const int32_t packet_data_size_limit = node1a.get_packet_size() - offsetof(struct packet_struct_data, chunk);
    // Restart at halfway point
    int32_t restart_run = floor(file_size_bytes / packet_data_size_limit)/2;

    int32_t packet_expected_total
        = num_files*ceil(file_size_bytes/packet_data_size_limit)   // number of DATA packets
        + num_files     // number of METADATA packets
        + 1             // number of QUEUE packets, 1 since node1 gets cut off, then twice for a full run after reboot, then one last one after tx_id[2] completes
        + 0             // number of REQDATA packets
        + 0             // number of REQMETA packets
        + num_files     // number of REQCOMPLETE packets
        + 0             // number of COMPLETE packets
        + 0;           // number of CANCEL packets
    // Perform first run to all-data-sent/write_meta point, then stop
    for (int runs=0; runs < restart_run; ++runs)
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
                node1a.receive_packet(rpacket);
            }
            respond = false;
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

    // Modify the meta files in the receiver side
    temp_dir= data_list_files(node1_name, "temp", "file");
    for (filestruc& file : temp_dir)
    {
        if (file.type == "meta")
        {
            FILE* f;
            f = fopen(file.path.c_str(), "rb+");
            if(f == nullptr)
            {
                return RW_ERROR_OPEN;
            }
            rewind(f);
            fwrite(&err_version_num, sizeof(uint8_t), 1, f);
            fclose(f);
        }
    }
    
    // Now start up node1b and resume file transfer
    // Load test nodeid table
    make_temp_nodeids();
    iretn = node2b.Init(cinfo2, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        // Restore old nodeids.ini file here in case test crashes
        restore_original_nodeids();
        return -1;
    }
    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();
    node2b.set_packet_size(PACKET_SIZE);

    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Change the version number to an erroneous one
            lpacket.data[offsetof(struct file_packet_header, version)] = err_version_num;

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
            if (iretn != TRANSFER_ERROR_VERSION)
            {
                debug_log.Printf("%5d | Mismatch version accepted? ver:%u type:%d size:%d limit:%d\n", __LINE__, unsigned(lpacket.data[0]), lpacket.header.type, lpacket.data.size(), PACKET_SIZE);
                goto endoftest;
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
                iretn = node1a.receive_packet(rpacket);
                if (iretn != TRANSFER_ERROR_VERSION)
                {
                    debug_log.Printf("%5d | Mismatch version accepted? ver:%u type:%d size:%d limit:%d\n", __LINE__, unsigned(rpacket.data[0]), rpacket.header.type, rpacket.data.size(), PACKET_SIZE);
                    goto endoftest;
                }
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
    if (sumv(packets_sent) != packet_expected_total)
    {
        debug_log.Printf("Verification fail: packet count mismatch. node1 sent: %d, node2 sent: %d, total packets sent: %d, expected packets sent: %d\n", packets_sent[NODE1], packets_sent[NODE2], sumv(packets_sent), packet_expected_total);
        --iretn;
    }

    // No new meta files created in node2's temp dir (i.e., node2_name/temp/file)
    // But node1 will still have its outgoing files and meta files, since no transaction was completed or cancelled
    iretn += test.verify_incoming_dir(node1_name, 0);
    iretn += test.verify_outgoing_dir(node2_name, num_files);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, num_files);

    // Outgoing queue for node1 will still have files
    if (node1a.outgoing_tx_recount(node2_name) != static_cast<int32_t>(num_files))
    {
        debug_log.Printf("Verification fail: queue check fail. node1a outgoing: %d\n", node1a.outgoing_tx_recount(node2_name));
        --iretn;
    }
    // Incoming queues for node2 will be empty
    if (node2b.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue check fail. node2b incoming: %d\n", node2b.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Sends every type of protocol packet to a node.
// Expect: No wrong-version packets will be accepted.
int32_t test_wrong_protocol_version2()
{
    int32_t iretn = 0;
    Transfer node1;

    // Initialize test parameters
    test_params test;
    iretn = test.init(node1_name, node2_name, 0, 0, __func__);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing test params %d\n", iretn);
        return iretn;
    }

    // Load nodeid table
    make_temp_nodeids();

    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node1.set_packet_size(PACKET_SIZE);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_packet_size(): %s\n", cosmos_error_string(iretn).c_str());
        return iretn;
    }

    // Restore old nodeids.ini file here in case test crashes
    restore_original_nodeids();

    vector<PacketComm> packets;

    tx_progress tx;
    tx.tx_id = 172;
    tx.file_crc = 12345;
    tx.file_name = "testfile";
    tx.file_size = 1000;
    tx.node_name = node1_name;
    tx.agent_name = "agent1";
    uint8_t chunk[217];
    vector<file_progress> holes = {{0,999}};
    uint8_t bad_ver = 255;

    PacketComm file_packet;
    file_packet.header.nodeorig = node2_id;
    file_packet.header.nodedest = node1_id;
    // QUEUE
    serialize_queue(file_packet, node2_id, {});
    packets.push_back(file_packet);
    // METADATA
    serialize_metadata(file_packet, node2_id, tx.tx_id, tx.file_crc, tx.file_name, tx.file_size, tx.agent_name);
    packets.push_back(file_packet);
    // DATA
    serialize_data(file_packet, node2_id, tx.tx_id, tx.file_crc, 217, 0, chunk);
    packets.push_back(file_packet);
    // REQCOMPLETE
    serialize_reqcomplete(file_packet, node2_id, tx.tx_id, tx.file_crc);
    packets.push_back(file_packet);
    // CANCEL
    serialize_cancel(file_packet, node2_id, tx.tx_id, PACKET_FILE_CRC_FORCE);
    packets.push_back(file_packet);
    // REQMETA
    serialize_reqmeta(file_packet, node2_id, tx.node_name, {tx.tx_id});
    packets.push_back(file_packet);
    // REQDATA
    serialize_reqdata(packets, node2_id, node1_id, tx.tx_id, tx.file_crc, holes, PACKET_SIZE);
    packets.push_back(file_packet);
    // COMPLETE
    serialize_complete(file_packet, node2_id, tx.tx_id, tx.file_crc);
    packets.push_back(file_packet);

    iretn = 0;
    for (auto packet : packets)
    {
        packet.data[0] = bad_ver;
        int32_t rcv_iretn = node1.receive_packet(packet);
        if (rcv_iretn != TRANSFER_ERROR_VERSION)
        {
            debug_log.Printf("%5d | Mismatch version not rejected. ver:%u|%u type:%u retval:%d\n", __LINE__, unsigned(packet.data[0]), unsigned(FILE_TRANSFER_PROTOCOL_VERSION), packet.header.type, rcv_iretn);
            --iretn;
        }
        node1_log.Printf("Packet type:%u returned: (%d) %s\n", packet.header.type, rcv_iretn, cosmos_error_string(rcv_iretn).c_str());
    }

    // Verify expected results
    iretn += test.verify_incoming_dir(node1_name, 0);
    iretn += test.verify_outgoing_dir(node1_name, 0);
    iretn += test.verify_incoming_dir(node2_name, 0);
    iretn += test.verify_outgoing_dir(node2_name, 0);
    iretn += test.verify_temp_dir(node1_name, 0);
    iretn += test.verify_temp_dir(node2_name, 0);

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(node2_name) || node1.incoming_tx_recount(node2_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node1 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node1.incoming_tx_recount(node2_name));
        --iretn;
    }

    return iretn;
}

// METADATA packet arrives a bit late.
// Also, since this nicely gets into the REQDATA section of get_outgoing_rpackets, will also
// stick in a crc fail case here too.
// Expect: All received chunks should continue to be received WITHOUT being discarded.
int32_t test_late_meta()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 3;
    double file_size_kib = 5.;
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
    make_temp_nodeids();

    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
    // Modify wait times to attempt to trigger REQDATA in DATA receive
    double waittime_sec = 2;
    iretn = node1.set_waittime(node2_name, 2, waittime_sec);
    iretn = node2.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
        return iretn;
    }
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
        + 4             // number of QUEUE packets, 2 for a usual run, once in the middle along with METADATA when requested, then one final one at the end
        + 2             // number of REQMETA packets, first is ignored, second after a REQCOMPLETE
        + 0             // number of REQDATA packets, could happen on imperfect transmission
        + num_files*2   // number of REQCOMPLETE packets, first after all DATA is sent, second after a wait
        + num_files     // number of COMPLETE packets
        + num_files     // number of CANCEL packets
        // That one file
        + ceil(file_size_bytes / packet_data_size_limit) // One of the files has all its DATA sent again
        + 2             // number of QUEUE packets, two for one last usual run
        + 2             // number of REQCOMPLETE packets, node1 thinks it's sent everything
        + 1             // number of REQMETA packets, node2 has just deleted the file as its crc was wrong
        + 1             // number of METADATA packets, node1 sends METADATA
        + 1             // number of REQDATA packets, sent after second REQCOMPLETE
        ;
    // Mess with a file in the middle of the run so that the file_crc check at the end fails for one of the files
    size_t tamper_file_run = ceil(file_size_bytes / packet_data_size_limit)/2;
    // Discard the first round of REQMETA packets
    int16_t hold_metadata = 2;
    int16_t num_responses = 4;
    for (size_t i=0; true; ++i)
    {
        if (i == tamper_file_run)
        {
            // Modify the one file on the receiver side
            vector<filestruc> temp_dir= data_list_files(node1_name, "temp", "file");
            for (filestruc& file : temp_dir)
            {
                if (file.type == "file")
                {
                    FILE* f;
                    f = fopen(file.path.c_str(), "rb+");
                    if(f == nullptr)
                    {
                        return RW_ERROR_OPEN;
                    }
                    rewind(f);
                    uint8_t bad_byte = 255;
                    fwrite(&bad_byte, sizeof(uint8_t), 16, f);
                    fclose(f);
                    break;
                }
            }
        }
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Discard all the METADATA packets until second REQMETA is received
            if (hold_metadata && lpacket.header.type == PacketComm::TypeId::DataFileMetaData)
            {
                continue;
            }
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
            if (--num_responses)
            {
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
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                // Discard all the METADATA packets until second REQMETA is received
                if (hold_metadata > 0 && rpacket.header.type == PacketComm::TypeId::DataFileReqMeta)
                {
                    --hold_metadata;
                }
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
    iretn += test.verify_incoming_dir(node1_name, num_files +1, true);
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

// Node 1 transfers multiple large files to Node 2, also the QUEUE packet arrives first
// Expect: Stuff to transfer
int32_t test_file_crc_check_fail()
{
    int32_t iretn = 0;
    Transfer node1, node2;
    size_t num_files = 1;
    double file_size_kib = 10.;
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
    make_temp_nodeids();

    iretn = node1.Init(cinfo1, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return iretn;
    }
    iretn = node2.Init(cinfo2, &node2_log);
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
    // Modify wait times to attempt to trigger REQDATA in DATA receive
    double waittime_sec = 1;
    iretn = node1.set_waittime(node2_name, 2, waittime_sec);
    iretn = node2.set_waittime(node1_name, 2, waittime_sec);
    if (iretn < 0)
    {
        debug_log.Printf("Error in set_waittime %d\n", iretn);
        return iretn;
    }
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
        = num_files*ceil(file_size_bytes / packet_data_size_limit)*2   // number of DATA packets
        + num_files*2     // number of METADATA packets
        + 3               // number of QUEUE packets, two for a usual run, then one at the end
        + num_files*2     // number of REQDATA packets (don't mind the minus here, calculation below goes num_files over the expected, so adjust here. Could be sent a couple times, depending on how long process takes)
        + num_files*2     // number of REQCOMPLETE packets
        + num_files     // number of COMPLETE packets
        + num_files;    // number of CANCEL packets
    // Mess with the file in the middle of the run so that the file_crc check at the end fails
    size_t tamper_file_run = ceil(file_size_bytes / packet_data_size_limit)/2;
    int16_t num_responses = 2;
    for (size_t i=0; true; ++i)
    {
        // Screw up the bytes, causing a crc check fail at the end
        if (i == tamper_file_run)
        {
            // Modify the files on the receiver side
            vector<filestruc> temp_dir= data_list_files(node1_name, "temp", "file");
            for (filestruc& file : temp_dir)
            {
                if (file.type == "file")
                {
                    FILE* f;
                    f = fopen(file.path.c_str(), "rb+");
                    if(f == nullptr)
                    {
                        return RW_ERROR_OPEN;
                    }
                    rewind(f);
                    uint8_t bad_byte = 255;
                    fwrite(&bad_byte, sizeof(uint8_t), 16, f);
                    fclose(f);
                }
            }
        }
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
            if (--num_responses)
            {
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
    iretn += test.verify_incoming_dir(node1_name, num_files*2, true);
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

void make_temp_nodeids()
{
    // Backup existing nodeids.ini file
//    nodeids_ini_path = get_cosmosnodes() + "nodeids.ini";
//    nodeids_ini_backup_path = get_cosmosnodes() + "nodeids.ini.back";
//    rename(nodeids_ini_path.c_str(), nodeids_ini_backup_path.c_str());

    // Load in some arbitrary node ids
//    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
//    temp_nodeids_ini << node1_id << " " << node1_name << "\n";
//    temp_nodeids_ini << node2_id << " " << node2_name << "\n";
//    temp_nodeids_ini.close();

    cinfo1->realm.name = "test";
    cinfo1->node.name = node1_name;
    cinfo1->realm.node_ids[node1_name] = node1_id;
    cinfo1->realm.node_ids[node2_name] = node2_id;

    cinfo2->realm.name = "test";
    cinfo2->node.name = node2_name;
    cinfo2->realm.node_ids[node1_name] = node1_id;
    cinfo2->realm.node_ids[node2_name] = node2_id;
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
        bytes[i] = rand() & 0xFF;
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
    vector<uint8_t> bytes;
    std::ofstream file_name;
    serialize_metafile(bytes, tx.tx_id, tx.file_crc, tx.file_name, tx.file_size, tx.node_name, tx.agent_name);
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

void handle_args(int argc, char *argv[])
{
    printf("Runs a series of tests over the file transfer protocol.\n");
    // Handle arguments
    if (argc == 1)
    {
        // If left blank, print everything
        printf("Use --help or -h to display usage.\n\n");
        return;
    }
    for(int i = 1; i < argc; i++){
        // Handle flags
        string arg(argv[i]);
        if (arg == "-h" || arg == "--help")
        {
            printf("Usage: monitor_adcs [-h|--help] [-v|--verbose] [-k|--keep-dir] [-p|--packet-size <PACKETSIZE>] [-t|--test <TESTNAME>]\n");
            printf("  -h or --help flag will print this help message.\n");
            printf("  -v or --verbose flag will print out the DATA packets to the debug log as well.\n");
            printf("  -k or --keep-dir flag will not delete the test folders after running.\n");
            printf("  -p or --packet-size flag will set the size limit of the packet for transmissions during the test. Defaults to 217.\n");
            printf("  -t or --test flag will have only the specified test be run.\n");
            printf("<TESTNAME> can be any of the following:\n");
            printf("  test_zero_size_files\n");
            printf("  test_large_files_and_queue\n");
            printf("  test_stop_resume\n");
            printf("  test_stop_resume2\n");
            printf("  test_packet_reqcomplete\n");
            printf("  test_many_files\n");
            printf("  test_packet_cancel_missed\n");
            printf("  test_bad_meta\n");
            printf("  test_chaotic_order\n");
            printf("  test_file_deleted_midrun\n");
            printf("  test_txid_overlap\n");
            printf("  test_wrong_protocol_version\n");
            printf("  test_wrong_protocol_version2\n");
            printf("  test_late_meta\n");
            printf("  test_file_crc_check_fail\n");
            printf("If no test is specified, then all tests will be run.\n");
            printf("Log files will be created in the ~/cosmos/nodes/ folder.\n");
            exit(0);
        }
        else if (arg == "-v" || arg == "--verbose")
        {
            verbose_log = true;
            printf("Enabled verbose logging.\n");
        }
        else if (arg == "-k" || arg == "--keep-dir")
        {
            remove_test_dirs = false;
            printf("Keeping test folders.\n");
        }
        else if ((arg == "-p" || arg == "--packet-size") && (i+1 < argc))
        {
            ++i;
            try
            {
                PACKET_SIZE = std::stoi(argv[i]);
            }
            catch (std::exception const &e)
            {
                std::cout << "Error: <packet_size> argument was invalid: " << e.what() << std::endl;
            }
            printf("Using packet size %u\n", PACKET_SIZE);
        }
        else if ((arg == "-t" || arg == "--test") && (i+1 < argc))
        {
            ++i;
            specified_test = string(argv[i]);
        }
    }
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
        if (!verbose_log)
        {
            return;
        }
    }

    if (debug_log->Type())
    {
        string node_name = lookup_node_id_name(cinfo1, packet.data[0]);
        uint8_t node_id = check_node_id(cinfo1, packet.data[0]);

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
                packet_struct_metadata meta;
                deserialize_metadata(packet.data, meta);
                debug_log->Printf("[METADATA] v%u %u %u %s ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_metadata, header.tx_id)], meta.file_name.c_str());
                break;
            }
        case PacketComm::TypeId::DataFileChunkData:
            {
                debug_log->Printf("[DATA] v%u %u %u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_data, header.tx_id)], packet.data[offsetof(packet_struct_data, chunk_start)]+256U*(packet.data[offsetof(packet_struct_data, chunk_start)+1]+256U*(packet.data[offsetof(packet_struct_data, chunk_start)+2]+256U*packet.data[offsetof(packet_struct_data, chunk_start)+3])), packet.data[offsetof(packet_struct_data, byte_count)]+256U*packet.data[offsetof(packet_struct_data, byte_count)+1]);
                break;
            }
        case PacketComm::TypeId::DataFileReqData:
            {
                debug_log->Printf("[REQDATA] v");
                for (auto& byte : packet.data)
                {
                    debug_log->Printf("%u ", unsigned(byte));
                }
                break;
            }
        case PacketComm::TypeId::DataFileReqComplete:
            {
                debug_log->Printf("[REQCOMPLETE] v%u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_reqcomplete, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileComplete:
            {
                debug_log->Printf("[COMPLETE] v%u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_complete, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileCancel:
            {
                debug_log->Printf("[CANCEL] v%u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_cancel, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileReqMeta:
        case PacketComm::TypeId::DataFileQueue:
            {
                packet_struct_queue queue;
                deserialize_queue(packet.data, queue);
                string label = packet.header.type == PacketComm::TypeId::DataFileReqMeta ? "REQMETA" : "QUEUE";
                debug_log->Printf("[%s] v%u %u ", label.c_str(), packet.data[offsetof(packet_struct_metadata, header.version)], node_id);
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
tx queue filling
new node_ids being added, node id mismatch between nodes
*/
