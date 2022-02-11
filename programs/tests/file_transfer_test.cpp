//! For testing the file transfer protocol implemented by transferclass and transferlib2
//!
//! Usage: file_transfer_test

#include "support/transferclass.h"
#include <fstream>


#define PACKET_SIZE 217

// Node names for the test transferclass's
const string node1_name = "transfer_test_node_1";
const string node2_name = "transfer_test_node_2";
const string tname3 = "transfer_test_node_3";

// CRC class for calculating crcs
CRC16 calc_crc;

// Non-debug logging
Error test_log;
int32_t test_count = 0;
int32_t err_count = 0;

// Debug logging to a file for more precise info
Error debug_log;
ElapsedTime tet, dt;

// Log node output to a file
Error node1_log, node2_log;

// nodeids.ini file path, and path for its backup
string nodeids_ini_path, nodeids_ini_backup_path;

// Helper functions
void cleanup();
int32_t create_file(int kib, string file_path);
void debug_packet(PacketComm packet, uint8_t direction, string type, Error* err_log);

// Tests
typedef int32_t (*test_func)();
void run_test(test_func, string test_name);
int32_t test_zero_size_files();
int32_t test_large_files();
int32_t test_stop_resume();
int32_t test_stop_resume2();

// main loop
int main(int argc, char *argv[])
{
    // Setup log paths and settings
    test_log.Set(Error::LOG_STDOUT_FFLUSH);
    debug_log.Set(Error::LOG_FILE_FFLUSH, get_cosmosnodes() + "/file_transfer_tests");
    node1_log.Set(Error::LOG_FILE_FFLUSH, get_cosmosnodes() + "/node1_transfer_test_log");
    node2_log.Set(Error::LOG_FILE_FFLUSH, get_cosmosnodes() + "/node2_transfer_test_log");
    nodeids_ini_path = get_cosmosnodes() + "/nodeids.ini";
    nodeids_ini_backup_path = get_cosmosnodes() + "/nodeids.ini.back";
    // Backup existing nodeids.ini file
    rename(nodeids_ini_path.c_str(), nodeids_ini_backup_path.c_str());


    //////////////////////////////////////////////////////////////////////////
    // Run tests
    //////////////////////////////////////////////////////////////////////////
    run_test(test_zero_size_files, "test_zero_size_files");
    run_test(test_large_files, "test_large_files");
    run_test(test_stop_resume, "test_stop_resume");
    //run_test(test_stop_resume2, "test_stop_resume2"); // Read the comments above the test_stop_resume2 function


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
//! \param \test A function pointer to an int32_t with no params
//! \param test_name What to log as the name of this test
//! \return n/a
void run_test(test_func test, string test_name)
{
    int32_t iretn;
    ++test_count;
    test_log.Printf("%-38s", ("Running " + test_name + "...").c_str());
    debug_log.Printf("%s\n", ("===== Running " + test_name + " =====").c_str());
    node1_log.Printf("%s\n", ("===== Running " + test_name + " =====").c_str());
    node2_log.Printf("%s\n", ("===== Running " + test_name + " =====").c_str());
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
    node1_log.Printf("\n");
    node2_log.Printf("\n");

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
    string agent_subfolder_name = __func__;

    // Check crcs before and after sending
    map<string, uint16_t> file_crcs;

    // arbitrary node_id's are chosen
    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
    temp_nodeids_ini << "33 " << node1_name << "\n";
    temp_nodeids_ini << "34 " << node2_name << "\n";
    temp_nodeids_ini.close();

    iretn = node1.Init(node1_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return -1;
    }
    iretn = node2.Init(node2_name);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return -1;
    }

    // Create files for node 1 to send to node 2
    string orig_out_dir = data_base_path(node2_name, "outgoing", agent_subfolder_name);
    string dest_in_dir = data_base_path(node1_name, "incoming", agent_subfolder_name);
    if (orig_out_dir.empty())
    {
        debug_log.Printf("Error creating orig_out_dir\n");
        return -1;
    }
    for (size_t i = 0; i < num_files; ++i)
    {
        string tfilename = "zero_" + std::to_string(i);
        iretn = create_file(file_size_kib, orig_out_dir + "/" + tfilename);
        file_crcs[tfilename] = calc_crc.calc_file(tfilename);
        if (iretn < 0)
        {
            return -1;
        }
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
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
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
    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (runs > runlimit)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. Runs: %d, runlimit: %d\n", runs, runlimit);
        --iretn;
    }

    // Zero-size files were ignored
    vector<filestruc> outgoing_dir= data_list_files(node2_name, "outgoing", agent_subfolder_name);
    vector<filestruc> incoming_dir = data_list_files(node2_name, "incoming", agent_subfolder_name);
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
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 transfers multiple large files to Node 2, no other tricks
int32_t test_large_files()
{
    int32_t iretn;
    Transfer node1, node2;
    double file_size_kib = 2;
    int32_t files_size_bytes = file_size_kib * 1024;
    
    size_t num_files = 3;
    string agent_subfolder_name = __func__;

    // Check crcs before and after sending
    map<string, uint16_t> file_crcs;

    // arbitrary node_id's are chosen
    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
    temp_nodeids_ini << "33 " << node1_name << "\n";
    temp_nodeids_ini << "34 " << node2_name << "\n";
    temp_nodeids_ini.close();

    iretn = node1.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return -1;
    }
    iretn = node2.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return -1;
    }

    // Create files for node 1 to send to node 2
    string orig_out_dir = data_base_path(node2_name, "outgoing", agent_subfolder_name);
    string dest_in_dir = data_base_path(node1_name, "incoming", agent_subfolder_name);
    if (orig_out_dir.empty())
    {
        debug_log.Printf("Error creating orig_out_dir\n");
        return -1;
    }
    for (size_t i = 0; i < num_files; ++i)
    {
        string tfilename = "large_" + std::to_string(i);
        iretn = create_file(file_size_kib, orig_out_dir + "/" + tfilename);
        file_crcs[tfilename] = calc_crc.calc_file(orig_out_dir + "/" + tfilename);
        if (iretn < 0)
        {
            return -1;
        }
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
    int32_t runs = 0;
    // Iteration estimation is file size / packet_size + 1, the plus one at the end is for the final COMPLETE/CANCEL handshake
    // Node 2 responds with COMPLETE packet immediately after last data packet, skipping the extra rounds of REQCOMPLETEs.
    // Add +1 for every round of REQCOMPLETEs you expect.
    int32_t runlimit = ceil(files_size_bytes / double(PACKET_SIZE)) + 1;
    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
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
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
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
    // Verify expected results
    iretn = 0;
    // Number of iteration matches estimate
    if (runs > runlimit)
    {
        debug_log.Printf("Verification fail: runlimit exceeded. Runs: %d, runlimit: %d\n", runs, runlimit);
        --iretn;
    }

    // File was successfully transferred
    vector<filestruc> outgoing_dir= data_list_files(node2_name, "outgoing", agent_subfolder_name);
    vector<filestruc> incoming_dir = data_list_files(node1_name, "incoming", agent_subfolder_name);
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
    if (node1.outgoing_tx_recount(node2_name) || node2.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue not empty. node1 outgoing: %d, node2 incoming: %d\n", node1.outgoing_tx_recount(node2_name), node2.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 starts transferring stuff to Node 2, node 1 stops, then resumes again
int32_t test_stop_resume()
{
    int32_t iretn;
    // First load, then stop
    Transfer node1a, node2a;
    // Second load after stop
    Transfer node1b;
    double file_size_kib = 2;
    int32_t files_size_bytes = file_size_kib * 1024;
    
    size_t num_files = 3;
    string agent_subfolder_name = __func__;

    // Check crcs before and after sending
    map<string, uint16_t> file_crcs;

    // arbitrary node_id's are chosen
    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
    temp_nodeids_ini << "33 " << node1_name << "\n";
    temp_nodeids_ini << "34 " << node2_name << "\n";
    temp_nodeids_ini.close();

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

    // Create files for node 1 to send to node 2
    string orig_out_dir = data_base_path(node2_name, "outgoing", agent_subfolder_name);
    string dest_in_dir = data_base_path(node1_name, "incoming", agent_subfolder_name);
    if (orig_out_dir.empty())
    {
        debug_log.Printf("Error creating orig_out_dir\n");
        return -1;
    }
    for (size_t i = 0; i < num_files; ++i)
    {
        string tfilename = "file_" + std::to_string(i);
        iretn = create_file(file_size_kib, orig_out_dir + "/" + tfilename);
        file_crcs[tfilename] = calc_crc.calc_file(orig_out_dir + "/" + tfilename);
        if (iretn < 0)
        {
            return -1;
        }
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
    int32_t runs = 0;
    // Iteration estimation is file size / packet_size + 1, the plus one at the end is for the final COMPLETE/CANCEL handshake
    // Node 2 responds with COMPLETE packet immediately after last data packet, skipping the extra rounds of REQCOMPLETEs.
    // Add +1 for every round of REQCOMPLETEs you expect.
    // We expect an additional 2 runs to complete from node restart. Once for REQCOMPLETE send, then second for final CANCEL send.
    int32_t runlimit_init = ceil(files_size_bytes / double(PACKET_SIZE)) + 1;
    int32_t runlimit = runlimit_init + 2;
    // Perform first run to all-data-sent/write_meta point, then stop
    while (runs < runlimit_init)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
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
            // Don't respond in first run
            // node2a.get_outgoing_rpackets(rpackets);
            // for (auto& rpacket : rpackets)
            // {
            //     debug_packet(rpacket, 1, "Outgoing", &node2_log);
            //     debug_packet(rpacket, 0, "Incoming", &node1_log);
            //     node1a.receive_packet(rpacket);
            // }
            // respond = false;
        }

        // break if runlimit is reached
        if (runs > runlimit)
        {
            break;
        }

        ++runs;
    }

    node1_log.Printf("------------------------\n--- Restarting node1 ---\n------------------------\n");
    node2_log.Printf("------------------------\n--- Restarting node1 ---\n------------------------\n");
    // Now start up node1b and resume file transfer
    iretn = node1b.Init(node1_name, &node1_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node1_name.c_str());
        return -1;
    }
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
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
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
            node2a.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1b.receive_packet(rpacket);
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
    vector<filestruc> outgoing_dir= data_list_files(node2_name, "outgoing", agent_subfolder_name);
    vector<filestruc> incoming_dir = data_list_files(node1_name, "incoming", agent_subfolder_name);
    if (outgoing_dir.size() > 0 || incoming_dir.size() != num_files)
    {
        debug_log.Printf("Verification fail: File count incorrect. node1a outgoing_dir: %d, node2a incoming_dir: %d\n", outgoing_dir.size(), incoming_dir.size());
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
    if (node1b.outgoing_tx_recount(node2_name) || node2a.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue check fail. node1b outgoing: %d, node2a incoming: %d\n", node1b.outgoing_tx_recount(node2_name), node2a.incoming_tx_recount(node1_name));
        --iretn;
    }

    return iretn;
}

// Node 1 starts transferring stuff to Node 2, node 2 stops, then resumes again after missing a few packets
// Note, this test works if you give the arg interval=0. to write_meta in DATA receive, but as it is,
// write_meta only happens every 5 seconds so it won't work by default.
int32_t test_stop_resume2()
{
    int32_t iretn;
    // First load, then stop
    Transfer node1a, node2a;
    // Second load after stop
    Transfer node2b;
    double file_size_kib = 2;
    int32_t files_size_bytes = file_size_kib * 1024;
    
    size_t num_files = 3;
    string agent_subfolder_name = __func__;

    // Check crcs before and after sending
    map<string, uint16_t> file_crcs;

    // arbitrary node_id's are chosen
    ofstream temp_nodeids_ini(nodeids_ini_path, std::ios::trunc);
    temp_nodeids_ini << "33 " << node1_name << "\n";
    temp_nodeids_ini << "34 " << node2_name << "\n";
    temp_nodeids_ini.close();

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

    // Create files for node 1 to send to node 2
    string orig_out_dir = data_base_path(node2_name, "outgoing", agent_subfolder_name);
    string dest_in_dir = data_base_path(node1_name, "incoming", agent_subfolder_name);
    if (orig_out_dir.empty())
    {
        debug_log.Printf("Error creating orig_out_dir\n");
        return -1;
    }
    for (size_t i = 0; i < num_files; ++i)
    {
        string tfilename = "file_" + std::to_string(i);
        iretn = create_file(file_size_kib, orig_out_dir + "/" + tfilename);
        file_crcs[tfilename] = calc_crc.calc_file(orig_out_dir + "/" + tfilename);
        if (iretn < 0)
        {
            return -1;
        }
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
    int32_t runs = 0;
    int miss = 2;
    // We expect an additional n+2 runs to complete from node restart, where n is however many packets for node2a to miss.
    // 1) node1 REQCOMPLETE send, node2b REQDATA send
    // n) node1 DATA send (for however many packets were missed), node2b COMPLETE send
    // n+1) node1 CANCEL send
    int32_t runlimit_init = (ceil(files_size_bytes / double(PACKET_SIZE)) + 1)/2;
    int32_t runlimit = ceil(files_size_bytes / double(PACKET_SIZE)) + (miss+2);
    // Perform first run to all-data-sent/write_meta point, then stop
    while (runs < runlimit_init)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Have node 2 receive all these packets, but skip a few
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
            if (runs < runlimit_init - miss)
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
            // Don't respond in first run
            // node2a.get_outgoing_rpackets(rpackets);
            // for (auto& rpacket : rpackets)
            // {
            //     debug_packet(rpacket, 1, "Outgoing", &node2_log);
            //     debug_packet(rpacket, 0, "Incoming", &node1_log);
            //     node1a.receive_packet(rpacket);
            // }
            // respond = false;
        }

        // break if runlimit is reached
        if (runs > runlimit)
        {
            break;
        }

        ++runs;
    }

    node1_log.Printf("------------------------\n--- Restarting node2 ---\n------------------------\n");
    node2_log.Printf("------------------------\n--- Restarting node2 ---\n------------------------\n");
    // Now start up node2b and resume file transfer
    iretn = node2b.Init(node2_name, &node2_log);
    if (iretn < 0)
    {
        debug_log.Printf("Error initializing %s\n", node2_name.c_str());
        return -1;
    }

    while (true)
    {
        lpackets.clear();
        // Get node 1's packets to send to node 2
        node1a.get_outgoing_lpackets(node2_name, lpackets);
        for (auto& lpacket : lpackets)
        {
            // Have node 2 receive all these packets
            debug_packet(lpacket, 1, "Outgoing", &node1_log);
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
            node2b.get_outgoing_rpackets(rpackets);
            for (auto& rpacket : rpackets)
            {
                debug_packet(rpacket, 1, "Outgoing", &node2_log);
                debug_packet(rpacket, 0, "Incoming", &node1_log);
                node1a.receive_packet(rpacket);
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
    vector<filestruc> outgoing_dir= data_list_files(node2_name, "outgoing", agent_subfolder_name);
    vector<filestruc> incoming_dir = data_list_files(node1_name, "incoming", agent_subfolder_name);
    if (outgoing_dir.size() > 0 || incoming_dir.size() != num_files)
    {
        debug_log.Printf("Verification fail: File count incorrect. node1a outgoing_dir: %d, node2b incoming_dir: %d\n", outgoing_dir.size(), incoming_dir.size());
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
    if (node1a.outgoing_tx_recount(node2_name) || node2b.incoming_tx_recount(node1_name))
    {
        debug_log.Printf("Verification fail: queue check fail. node1a outgoing: %d, node2b incoming: %d\n", node1a.outgoing_tx_recount(node2_name), node2b.incoming_tx_recount(node1_name));
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
    //rename((get_cosmosnodes() + "/" + node1_name).c_str(), temp_del_path.c_str());
    //rename((get_cosmosnodes() + "/" + node2_name).c_str(), temp_del_path.c_str());
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

//! For printing out debug statements about incoming and outgoing packets.
//! \param packet An incoming or outgoing packet
//! \param direction PACKET_IN or PACKET_OUT
//! \param type Incoming or outgoing, used only in the print statement
//! \param err_log Pointer to Error object to log with
//! \return n/a
void debug_packet(PacketComm packet, uint8_t direction, string type, Error* err_log)
{
    if (err_log->Type())
    {
        string node_name = NodeData::lookup_node_id_name(packet.data[0]);
        uint8_t node_id = NodeData::check_node_id(packet.data[0]);

        if (direction == 0)
        {
            err_log->Printf("%.4f %.4f RECV L %u R %u %s [%s] Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), type.c_str(), packet.data.size());
        }
        else if (direction == 1)
        {
            err_log->Printf("%.4f %.4f SEND L %u R %u %s [%s] Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), type.c_str(), packet.data.size());
        }

        switch (packet.header.type)
        {
        case PacketComm::TypeId::FileMetaData:
            {
                string file_name(&packet.data[PACKET_METASHORT_OFFSET_FILE_NAME], &packet.data[PACKET_METASHORT_OFFSET_FILE_NAME+TRANSFER_MAX_FILENAME]);
                err_log->Printf("[METADATA] %u %u %s ", node_id, packet.data[PACKET_METASHORT_OFFSET_TX_ID], file_name.c_str());
                break;
            }
        case PacketComm::TypeId::FileChunkData:
            {
                err_log->Printf("[DATA] %u %u %u %u ", node_id, packet.data[PACKET_DATA_OFFSET_TX_ID], packet.data[PACKET_DATA_OFFSET_CHUNK_START]+256U*(packet.data[PACKET_DATA_OFFSET_CHUNK_START+1]+256U*(packet.data[PACKET_DATA_OFFSET_CHUNK_START+2]+256U*packet.data[PACKET_DATA_OFFSET_CHUNK_START+3])), packet.data[PACKET_DATA_OFFSET_BYTE_COUNT]+256U*packet.data[PACKET_DATA_OFFSET_BYTE_COUNT+1]);
                break;
            }
        case PacketComm::TypeId::FileReqData:
            {
                err_log->Printf("[REQDATA] %u %u %u %u ", node_id, packet.data[PACKET_REQDATA_OFFSET_TX_ID], packet.data[PACKET_REQDATA_OFFSET_HOLE_START]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_START+1]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_START+2]+256U*packet.data[PACKET_REQDATA_OFFSET_HOLE_START+3])), packet.data[PACKET_REQDATA_OFFSET_HOLE_END]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_END+1]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_END+2]+256U*packet.data[PACKET_REQDATA_OFFSET_HOLE_END+3])));
                break;
            }
        case PacketComm::TypeId::FileReqComplete:
            {
                err_log->Printf("[REQCOMPLETE] %u %u ", node_id, packet.data[PACKET_REQCOMPLETE_OFFSET_TX_ID]);
                break;
            }
        case PacketComm::TypeId::FileComplete:
            {
                err_log->Printf("[COMPLETE] %u %u ", node_id, packet.data[PACKET_COMPLETE_OFFSET_TX_ID]);
                break;
            }
        case PacketComm::TypeId::FileCancel:
            {
                err_log->Printf("[CANCEL] %u %u ", node_id, packet.data[PACKET_CANCEL_OFFSET_TX_ID]);
                break;
            }
        case PacketComm::TypeId::FileReqMeta:
            {
                err_log->Printf("[REQMETA] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (packet.data[PACKET_REQMETA_OFFSET_TX_ID+i])
                    {
                        err_log->Printf("%u ", packet.data[PACKET_REQMETA_OFFSET_TX_ID+i]);
                    }
                break;
            }
        case PacketComm::TypeId::FileReqQueue:
            {
                err_log->Printf("[REQQUEUE] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
            }
            break;
        case PacketComm::TypeId::FileQueue:
            {
                err_log->Printf("[QUEUE] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
                // Note: this assumes that PACKET_QUEUE_FLAGS_TYPE is a uint16_t type
                for (PACKET_QUEUE_FLAGS_TYPE i=0; i<PACKET_QUEUE_FLAGS_LIMIT; ++i)
                {
                    PACKET_QUEUE_FLAGS_TYPE flags = uint16from(&packet.data[PACKET_QUEUE_OFFSET_TX_ID+(2*i)], ByteOrder::LITTLEENDIAN);
                    err_log->Printf("%u ", flags);
                }
            }
            break;
        case PacketComm::TypeId::FileHeartbeat:
            {
                err_log->Printf("[HEARTBEAT] %u %s %hu %u %u", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)], packet.data[PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD]
                        , packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+1]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+2]+256U*packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+3]))
                        , packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+1]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+2]+256U*packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+3])));
                break;
            }
        case PacketComm::TypeId::FileMessage:
            {
                err_log->Printf("[MESSAGE] %u %hu %s", node_id, packet.data[PACKET_MESSAGE_OFFSET_LENGTH], &packet.data[PACKET_MESSAGE_OFFSET_BYTES]);
                break;
            }
        case PacketComm::TypeId::FileCommand:
            {
                err_log->Printf("[COMMAND] %u %hu %s", node_id, packet.data[PACKET_COMMAND_OFFSET_LENGTH], &packet.data[PACKET_COMMAND_OFFSET_BYTES]);
                break;
            }
        default:
            {
                err_log->Printf("[OTHER] %u %s", node_id, "Non-file transfer type in packet.header.type");
            }
        }
        err_log->Printf("\n");
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
*/
