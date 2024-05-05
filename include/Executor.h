#include <stdint.h>
#include <filesystem>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <random>

namespace fs = std::filesystem;

#ifndef EXECUTOR_H
#define EXECUTOR_H

class Executor {
public:
    explicit Executor(std::vector<fs::directory_entry> entries, std::string directory) : 
    entries_(entries), directory_(directory) {
        num_processors_ = entries.size();
        trace_lines_.resize(num_processors_);
    }

    void Init() {
        for(int i = 0; i < num_processors_; i++) {
            // Reset the trace file line numbers
            trace_position_[i] = 0;

            // Read the trace file into a vector
            std::ifstream file_stream(directory_ + entries_[i].path().filename().string());
            std::string line;
            while (std::getline(file_stream, line)) {
                trace_lines_[i].push_back(line);
            }
            file_stream.close();
        }
    }

    bool Next(uint64_t &processor_id, char &action, uint64_t &address) {
        // Have exhausted all the trace file for all the processors
        if(finished_ == entries_.size()) {
            return false;
        }

        std::random_device rd; 
        std::mt19937 gen(rd()); 
        std::uniform_int_distribution<> distrib(0, num_processors_ - 1);

        processor_id = distrib(gen);

        // Keep moving to the right
        while(trace_lines_[processor_id].size() == trace_position_[processor_id]) {
            processor_id = (processor_id + 1) % num_processors_;
        }

        std::string next_instruction = trace_lines_[processor_id][trace_position_[processor_id]++];

        std::vector<std::string> v(3);
        parse(next_instruction, v);

        /*std::cout << "Processor " << processor_id << " executes " << v[0] << " at address"
        << v[1] << " of size " << v[2] << std::endl;*/

        action = v[0][0];
        size_t pos;
        address = std::stoll(v[1], &pos, 16);


        // Update finished count if the trace is exhausted
        if(trace_lines_[processor_id].size() == trace_position_[processor_id]) {
            ++finished_;
        }

        return true;
    }

private:
    int finished_ = 0;
    std::vector<fs::directory_entry> entries_;
    std::vector<std::vector<std::string>> trace_lines_;
    std::unordered_map<int, int> trace_position_; 
    int num_processors_;
    std::string directory_;

    void parse(std::string &next_instruction, std::vector<std::string> &v) {
        v[0] = next_instruction[0];
        uint64_t comma_index = next_instruction.find(',');
        v[1] = next_instruction.substr(2, comma_index - 2);
        v[2] = next_instruction.substr(comma_index + 1);
    }
};


#endif // EXECUTOR_H