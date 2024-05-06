#include "MESIFCache.h"

#ifndef MESIF_COHERENCE_H
#define MESIF_COHERENCE_H


/*
This class maintains the coherence across N L1 caches that are private to N processors
and a common L2 cache
*/

#define LOCAL_L1_FETCH 10
#define OTHER_L1_FETCH 30
#define BROADCAST_LOOKUP 30
#define L2_FETCH 100
#define DRAM_FETCH 500

class MESIFCoherence {
public:
    explicit MESIFCoherence(int num_processors, uint64_t num_lines, uint64_t L1s, uint64_t L2s, uint64_t b, uint64_t E):
    num_processors_(num_processors) {
        
        for(int i = 0; i < num_processors; i++) {
            caches_.emplace_back(num_lines, E, L1s, b);
        }

        // Create an L2 cache
        L2_cache_.emplace_back(num_lines * 4, E, L2s, b);

        // Initialize cost
        cost_ = 0;
    }

    void Load(uint64_t processor_id, uint64_t address);

    void Store(uint64_t processor_id, uint64_t address);

    uint64_t GetCost();

private:
// 0 to (num_processors - 1) : L1
// (num_processors) : L2
std::vector<MESIFCache> caches_;
std::vector<MESIFCache> L2_cache_;
uint64_t num_processors_; // No. of processors
uint64_t cost_; // cost for fetching from L1 or L2
};

#endif // MESIF_COHERENCE_H