#include "POPSCache.h"
#include "bloom_filter.hpp"

#ifndef POPS_COHERENCE_H
#define POPS_COHERENCE_H

#define LOCAL_L1_FETCH 10
#define OTHER_L1_FETCH 30
#define BLOOM_FILTER_LOOKUP 50
#define L2_FETCH 100
#define DRAM_FETCH 500

class POPSCoherence {
public:
    explicit POPSCoherence(int num_processors, uint64_t num_lines, uint64_t L1s, uint64_t L2s, uint64_t b, uint64_t E) :
    num_processors_(num_processors), num_lines_(num_lines), E_(E), b_(b) {
        for(int i = 0; i < num_processors; i++) {
            caches_.emplace_back(num_lines, E, L1s, b);
        }

        // Create an L2 cache
        L2_cache_.emplace_back(num_lines * 4, E, L2s, b);

        // Initialize cost
        cost_ = 0;

        bloom_parameters parameters;
        parameters.projected_element_count = 1000;
        parameters.false_positive_probability = 0.0001;

        // Global bloom filter data structure
        for(int i = 0; i <= num_processors; i++) {
            bloom_filters_.emplace_back(parameters);
        }
    }
    void Load(uint64_t processor_id, uint64_t address);

    void Store(uint64_t processor_id, uint64_t address);

    uint64_t GetCost();

private:

std::vector<POPSCache> caches_;
std::vector<POPSCache> L2_cache_;
uint64_t num_processors_; // No. of processors
uint64_t num_lines_; // No. of lines in the L1 cache
uint64_t E_; // Set size for the cache
uint64_t b_; // Block size for the cache
uint64_t cost_; // cost for fetching from L1 or L2
std::vector<bloom_filter> bloom_filters_;
};

#endif