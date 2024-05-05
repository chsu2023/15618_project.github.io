#include "MESIFCache.h"

#ifndef MESIF_COHERENCE_H
#define MESIF_COHERENCE_H


/*
This class maintains the coherence across N L1 caches that are private to N processors
and a common L2 cache
*/

#define LOCAL_L1_FETCH 10
#define OTHER_L1_FETCH 30
#define L2_FETCH 100
#define DRAM_FETCH 500

class MESIFCoherence {
public:
    explicit MESIFCoherence(int num_processors, int64_t num_lines, int64_t L1s, int64_t L2s, int64_t b, int64_t E):
    num_lines_(num_lines), L1s_(L1s), L2s_(L1s), E_(E), b_(b) {
        
        for(int i = 0; i < num_processors; i++) {
            caches_.emplace_back(num_lines, E, L1s, b);
        }

        // Create an L2 cache
        L2_cache_.emplace_back(num_lines * 4, E, L2s, b);

        // Initialize cost
        cost_ = 0;
    }

    void Load(int64_t processor_id, int64_t address);

    void Store(int64_t processor_id, int64_t address);

    int64_t GetCost();

private:
// 0 to (num_processors - 1) : L1
// (num_processors) : L2
std::vector<MESIFCache> caches_;
std::vector<MESIFCache> L2_cache_;
int64_t num_processors_; // No. of processors
int64_t num_lines_; // No. of lines in the L1 cache
int64_t L1s_; // No. of L1s same as the number of processors
int64_t L2s_; // Common L2 cache for all the processors
int64_t E_; // Set size for the cache
int64_t b_; // Block size for the cache
int64_t cost_; // cost for fetching from L1 or L2
};

#endif // MESIF_COHERENCE_H