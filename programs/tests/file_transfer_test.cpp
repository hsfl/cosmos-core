//! For testing the file transfer protocol implemented by transferclass and transferlib2
//!
//! Usage: file_transfer_test

#include "support/transferclass.h"
#include <fstream>


#define PACKET_SIZE 217

// Node names for the test transferclass's
const string tname1 = "transfer_test_node_1";
const string tname2 = "transfer_test_node_2";
const string tname3 = "transfer_test_node_3";

// CRC class for calculating crcs
CRC16 calc_crc;

// Non-debug logging
Error test_log;
int32_t test_count = 0;
int32_t err_count = 0;

// Debug logging to a file for more precise info
Error debug_log;
ElapsedTime tet;

// nodeids.ini file path, and path for its backup
string nodeids_ini_path, nodeids_ini_backup_path;

// Helper functions
void cleanup();
int32_t create_file(int kib, string file_path);
void debug_packet(PacketComm packet, uint8_t direction, string type, int32_t use_channel);

// Tests
typedef int32_t (*test_func)();
void run_test(test_func, string test_name);
int32_t test_zero_size_files();
int32_t test_large_files();

// main loop
int main(int argc, char *argv[])
{
    test_log.Set(Error::LOG_STDOUT_FFLUSH);
    debug_log.Set(Error::LOG_FILE_FFLUSH, get_cosmosnodes() + "/file_transfer_tests");
    debug_log.Open();
    nodeids_ini_path = get_cosmosnodes() + "/nodeids.ini";
    nodeids_ini_backup_path = get_cosmosnodes() + "/nodeids.ini.back";
    // Backup existing nodeids.ini file
    rename(nodeids_ini_path.c_str(), nodeids_ini_backup_path.c_str());


    //////////////////////////////////////////////////////////////////////////
    // Run tests
    //////////////////////////////////////////////////////////////////////////
    run_test(test_zero_size_files, "test_zero_size_files");
    run_test(test_large_files, "test_large_files");


    //////////////////////////////////////////////////////////////////////////
    // Clean up
    //////////////////////////////////////////////////////////////////////////

    // Restore backup of nodeids.ini file
    rename(nodeids_ini_backup_path.c_str(), nodeids_ini_path.c_str());
    debug_log.Printf("%s\n", "Cleaning up.");
    cleanup();


    //////////////////////////////////////////////////////////////////////////
    // Exit
    //////////////////////////////////////////////////////////////////////////

    // Final verdict
    if (!err_count)
    {
        test_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
        debug_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
    }
    else
    {
        test_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
        debug_log.Printf("\n%-25sSuccess: %d, Failed: %d\n", "All tests completed.", (test_count - err_count), err_count);
    }

    test_log.Printf("Exiting...\n");
    debug_log.Printf("Exiting...\n\n");

    test_log.Close();
    debug_log.Close();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Tests
//////////////////////////////////////////////////////////////////////////////

//! Run a test function
//! \param \test A function pointer to an int32_t with no params
//! \param test_name What to log as the name of this test
//! \return n/a
void run_test(test_func test, string test_name)
{
    int32_t iretn;
    ++test_count;
    test_log.Printf("%-38s", ("Running " + test_name + "...").c_str());
    debug_log.Printf("%s\n", ("===== Running " + test_name + " =====").c_str());
    iretn = test();
    if (iretn < 0)
    {
        test_log.Printf("...encountered error.\n");
        debug_log.Printf("Test %s encountered error.\n", test_name.c_str());
        ++err_count;
    }
    else
    {
        test_log.Printf("...success.\n");
        debug_log.Printf("Test %s success.\n", test_name.c_str());
    }

    cleanup();
    debug_log.Printf("\n");
}

// Node 1 attempts to transfer zero-size files to node 2
int32_t test_zero_size_files()
{
    int32_t iretn;
    Transfer node1, node2;
    double file_size_kib = 0;
    int32_t files_size_bytes = file_size_kib * 1024;
    
    size_t num_files = 3;
    string agent_subfolder_name = "test_zero_size_files";

    // Check crcs before and after sending
    map<string, uint16_t> file_crcs;

    // arbitrary node_id's are chosen
    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
    temp_nodeids_ini << "33 " << tname1 << "\n";
    temp_nodeids_ini << "34 " << tname2 << "\n";
    temp_nodeids_ini.close();

    iretn = node1.Init(tname1);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", tname1.c_str());
        return -1;
    }
    iretn = node2.Init(tname2);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", tname2.c_str());
        return -1;
    }

    // Create files for node 1 to send to node 2
    string dest_out_dir = data_base_path(tname2, "outgoing", agent_subfolder_name);
    string dest_in_dir = data_base_path(tname2, "incoming", agent_subfolder_name);
    if (dest_out_dir.empty())
    {
        debug_log.Printf("Error creating dest_out_dir\n");
        return -1;
    }
    for (size_t i = 0; i < num_files; ++i)
    {
        string tfilename = "zero_" + std::to_string(i);
        iretn = create_file(file_size_kib, dest_out_dir + "/" + tfilename);
        file_crcs[tfilename] = calc_crc.calc_file(tfilename);
        if (iretn < 0)
        {
            return -1;
        }
    }

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Start transfer process
    node1.outgoing_tx_load(tname2);
    int32_t runs = 0;
    int32_t runlimit = ceil(files_size_bytes / double(PACKET_SIZE)) + 1;
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(tname2, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Have node 2 receive all these packets
            iretn = node2.receive_packet(lpacket);
            if (iretn == node2.RESPONSE_REQUIRED)
            {
                respond = true;
                
            }
        }

        // break if transfers stop
        debug_log.Printf("lpackets.size(): %d\n", lpackets.size());
        if ((!lpackets.size() && !respond))
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%4d | lpackets.size(): %d, respond: %s, runs: %d, runlimit: %d\n", __LINE__, lpackets.size(), rs.c_str(), runs, runlimit);
            break;
        }
        if (runs > runlimit)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%4d | lpackets.size(): %d, respond: %s, runs: %d, runlimit: %d\n", __LINE__, lpackets.size(), rs.c_str(), runs, runlimit);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
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
    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (runs > runlimit)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. Runs: %d, runlimit: %d\n", runs, runlimit);
        --iretn;
    }

    // Zero-size files were ignored
    vector<filestruc> outgoing_dir= data_list_files(tname2, "outgoing", agent_subfolder_name);
    vector<filestruc> incoming_dir = data_list_files(tname2, "incoming", agent_subfolder_name);
    if (outgoing_dir.size() != num_files || incoming_dir.size() != 0)
    {
        debug_log.Printf("Verification fail: File count incorrect. node1 outgoing_dir: %d, node2 incoming_dir: %d\n", outgoing_dir.size(), incoming_dir.size());
        --iretn;
    }
    for (filestruc& file : incoming_dir)
    {
        if (file.size != files_size_bytes)
        {
            debug_log.Printf("Verification fail: File size error. %s %d:%d\n", file.name.c_str(), file.size, files_size_bytes);
            --iretn;
        }
        if (file_crcs.find(file.name) == file_crcs.end())
        {
            debug_log.Printf("Verification fail: File name error. %s %d:%d\n", file.name.c_str(), file.size, files_size_bytes);
            --iretn;
        }
        else 
        {
            uint16_t crc_recv = calc_crc.calc_file(dest_in_dir + "/" + file.name);
            if (file_crcs[file.name] != crc_recv)
            {
                debug_log.Printf("Verification fail: CRC mismatch. %s %d:%d\n", file.name.c_str(), file_crcs[file.name], crc_recv);
                --iretn;
            }
        }
    }

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(tname2) || node2.incoming_tx_recount(tname1))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(tname2), node2.incoming_tx_recount(tname1));
        --iretn;
    }

    return iretn;
}

// Node 1 transfers multiple large files to Node 2, no other tricks
int32_t test_large_files()
{
    int32_t iretn;
    Transfer node1, node2;
    double file_size_kib = 1;
    int32_t files_size_bytes = file_size_kib * 1024;
    
    size_t num_files = 3;
    string agent_subfolder_name = "test_large_files";

    // Check crcs before and after sending
    map<string, uint16_t> file_crcs;

    // arbitrary node_id's are chosen
    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
    temp_nodeids_ini << "33 " << tname1 << "\n";
    temp_nodeids_ini << "34 " << tname2 << "\n";
    temp_nodeids_ini.close();

    iretn = node1.Init(tname1);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", tname1.c_str());
        return -1;
    }
    iretn = node2.Init(tname2);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", tname2.c_str());
        return -1;
    }

    // Create files for node 1 to send to node 2
    string dest_out_dir = data_base_path(tname2, "outgoing", agent_subfolder_name);
    string dest_in_dir = data_base_path(tname2, "incoming", agent_subfolder_name);
    if (dest_out_dir.empty())
    {
        debug_log.Printf("Error creating dest_out_dir\n");
        return -1;
    }
    for (size_t i = 0; i < num_files; ++i)
    {
        string tfilename = "large_" + std::to_string(i);
        iretn = create_file(file_size_kib, dest_out_dir + "/" + tfilename);
        file_crcs[tfilename] = calc_crc.calc_file(dest_out_dir + "/" + tfilename);
        if (iretn < 0)
        {
            return -1;
        }
    }

    vector<PacketComm> lpackets, rpackets;
    bool respond = false;
    // Start transfer process
    node1.outgoing_tx_load(tname2);
    int32_t runs = 0;
    // Iteration estimation is file size / packet_size + 1, the plus one at the end is for the final COMPLETE/CANCEL handshake
    // Node 2 responds with COMPLETE packet immediately after last data packet, skipping the extra rounds of REQCOMPLETEs,
    // add +1 for every round of REQCOMPLETEs you expect
    int32_t runlimit = ceil(files_size_bytes / double(PACKET_SIZE)) + 1;
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(tname2, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Have node 2 receive all these packets
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
            debug_log.Printf("%4d | lpackets.size(): %d, respond: %s, runs: %d, runlimit: %d\n", __LINE__, lpackets.size(), rs.c_str(), runs, runlimit);
            break;
        }
        if (runs > runlimit)
        {
            string rs = respond ? "true" : "false";
            debug_log.Printf("%4d | lpackets.size(): %d, respond: %s, runs: %d, runlimit: %d\n", __LINE__, lpackets.size(), rs.c_str(), runs, runlimit);
        }

        if (respond)
        {
            rpackets.clear();
            node2.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
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
    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (runs > runlimit)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. Runs: %d, runlimit: %d\n", runs, runlimit);
        --iretn;
    }

    // File was successfully transferred
    vector<filestruc> outgoing_dir= data_list_files(tname2, "outgoing", agent_subfolder_name);
    vector<filestruc> incoming_dir = data_list_files(tname2, "incoming", agent_subfolder_name);
    if (outgoing_dir.size() > 0 || incoming_dir.size() != num_files)
    {
        debug_log.Printf("Verification fail: File count incorrect. node1 outgoing_dir: %d, node2 incoming_dir: %d\n", outgoing_dir.size(), incoming_dir.size());
        --iretn;
    }
    for (filestruc& file : incoming_dir)
    {
        if (file.size != files_size_bytes)
        {
            debug_log.Printf("Verification fail: File size error. %s %d:%d\n", file.name.c_str(), file.size, files_size_bytes);
            --iretn;
        }
        if (file_crcs.find(file.name) == file_crcs.end())
        {
            debug_log.Printf("Verification fail: File name error. %s %d:%d\n", file.name.c_str(), file.size, files_size_bytes);
            --iretn;
        }
        else 
        {
            uint16_t crc_recv = calc_crc.calc_file(dest_in_dir + "/" + file.name);
            if (file_crcs[file.name] != crc_recv || !crc_recv)
            {
                debug_log.Printf("Verification fail: CRC mismatch. %s %d:%d\n", file.name.c_str(), file_crcs[file.name], crc_recv);
                --iretn;
            }
        }
    }

    // Outgoing/incoming queues are empty
    if (node1.outgoing_tx_recount(tname2) || node2.incoming_tx_recount(tname1))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(tname2), node2.incoming_tx_recount(tname1));
        --iretn;
    }

    return iretn;
}

//////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////

void cleanup()
{
    debug_log.Printf("Removing created test directories... ");
    
    // Delete created folders, don't touch this
    const string temp_del_path = get_cosmosnodes() + "/safe_to_delete";
    //rename((get_cosmosnodes() + "/" + tname1).c_str(), temp_del_path.c_str());
    //rename((get_cosmosnodes() + "/" + tname2).c_str(), temp_del_path.c_str());
    //rename((get_cosmosnodes() + "/" + tname3).c_str(), temp_del_path.c_str());
    // (In)sanity checks before running rm -r
    if (data_isdir(temp_del_path)
    && std::count(temp_del_path.begin(), temp_del_path.end(), '/') > 3
    && !std::count(temp_del_path.begin(), temp_del_path.end(), ' ')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '\t')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '\v')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '\n')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '\r')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '\\')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '|')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '-')
    && !std::count(temp_del_path.begin(), temp_del_path.end(), '.'))
    {
        //data_execute("rm -r " + temp_del_path);
        debug_log.Printf("... ");
    }
    debug_log.Printf("OK.\n");
}

// Create an all-zero-char file of kib kibibytes at the file_path
// kb: kibibytes
// Returns 0 on success, negative on error
int32_t create_file(int kib, string file_path)
{
    vector<char> zeros(1024, 0);
    ofstream of(file_path, std::ios::binary | std::ios::out);
    for(int i = 0; i < kib; ++i)
    {
        if (!of.write(&zeros[0], zeros.size()))
        {
            debug_log.Printf("Error creating %s", file_path.c_str());
            return -1;
        }
    }
    return 0;
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
*/
