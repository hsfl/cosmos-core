#ifndef __FILE_IT_HELPERS_H__
#define __FILE_IT_HELPERS_H__

//////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////
#include "support/datalib.h"

namespace Cosmos {
namespace Test {
namespace Integration {
namespace File {
    //! Set up some nodeids for the test to use
    void load_temp_nodeids(uint8_t num_agents)
    {
        // Load in some arbitrary node ids
        for (size_t i=0; i<num_agents; ++i)
        {
            std::string node_name = ".tnode_" + std::to_string(i);
            NodeData::node_ids[node_name] = i;
        }
    }

    void cleanup(uint8_t num_agents)
    {
        // Delete created folders, don't touch this
        for (size_t i=0; i<num_agents; ++i)
        {
            std::string node_name = ".tnode_" + std::to_string(i);
            rmdir((get_cosmosnodes() + node_name).c_str());
        }
    }

    void rmdir(const std::string dirpath)
    {
        // (In)sanity checks before running rm -r
        if (data_isdir(dirpath)
        && std::count(dirpath.begin(), dirpath.end(), '/') > 3
        && !std::count(dirpath.begin(), dirpath.end(), ' ')
        && !std::count(dirpath.begin(), dirpath.end(), '\t')
        && !std::count(dirpath.begin(), dirpath.end(), '\v')
        && !std::count(dirpath.begin(), dirpath.end(), '\n')
        && !std::count(dirpath.begin(), dirpath.end(), '\r')
        && !std::count(dirpath.begin(), dirpath.end(), '\\')
        && !std::count(dirpath.begin(), dirpath.end(), '|')
        && !std::count(dirpath.begin(), dirpath.end(), '-')
        && !std::count(dirpath.begin(), dirpath.end(), '.'))
        {
            data_execute("rm -r " + dirpath);
        }
    }

    // Create an all-zero-char file of kib kibibytes at the file_path
    // kb: kibibytes
    // Returns 0 on success, negative on error
    int32_t create_file(int32_t kib, std::string file_path)
    {
        std::vector<char> zeros(1024, 0);
        std::ofstream of(file_path, std::ios::binary | std::ios::out);
        for(int i = 0; i < kib; ++i)
        {
            if (!of.write(&zeros[0], zeros.size()))
            {
                return -1;
            }
        }
        return 0;
    }
} // End namespace File
} // End namespace Integration
} // End namespace Tests
} // End namespace Cosmos

#endif // end __FILE_IT_HELPERS_H__