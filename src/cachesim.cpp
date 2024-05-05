#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include "Executor.h"
#include "MSICoherence.h"
#include "MESIFCoherence.h"
#include "POPSCoherence.h"

namespace fs = std::filesystem;

int main(int argc, char * argv[]) {
    if(argc != 2) {
        std::cout << "Usage ./cachesim /path/to/traces/" << std::endl;
        return -1;
    }

    fs::path traces_directory = argv[1];
    std::vector<fs::directory_entry> entries;
    // Check if the directory exists
    if (fs::exists(traces_directory) && fs::is_directory(traces_directory)) {
        // Iterate over the files in the directory
        for (const auto& entry : fs::directory_iterator(traces_directory)) {
            entries.push_back(entry);
        }
    } else {
        std::cerr << "Directory not found or is not a directory." << std::endl;
    }

    Executor exec(entries, argv[1]);
    exec.Init();

    uint64_t processor_id;
    char action;
    uint64_t address;

    //MSICoherence controller(4, 256, 4, 1, 6, 6);
    //MESIFCoherence controller(4, 256, 4, 1, 6, 6);
    POPSCoherence controller(4, 256, 4, 1, 6, 6);

    while(exec.Next(processor_id, action, address)) {
        if(action == 'L') {
            //std::cout << "Loading address 0x" << std::hex << address << " on processor " << processor_id << std::endl;
            controller.Load(processor_id, address);
        } else {
            //std::cout << "Storing address 0x" << std::hex << address << " on processor " << processor_id << std::endl;
            controller.Store(processor_id, address);
        }
    }

    std::cout << "Cost of transactions : " << controller.GetCost() << std::endl; 
    return 0;
}