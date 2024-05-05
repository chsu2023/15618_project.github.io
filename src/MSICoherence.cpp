#include "MSICoherence.h"

void MSICoherence::Load(uint64_t processor_id, uint64_t address) {

    // Check for presence in the local cache
    if(caches_[processor_id].IsPresent(address)) {
        // No need to check the state
        cost_ += LOCAL_L1_FETCH;
        return;
    }

    
    // Check for presence in other L1 caches
    bool found_in_other_L1 = false;
    for(uint64_t i = 0; i < num_processors_; i++) {
        if(i != processor_id && caches_[i].IsPresent(address)) {
            found_in_other_L1 = true;
            // Check the state
            if(caches_[i].IsModified(address)) {
                found_in_other_L1 = false; // evict and load from L2
                caches_[i].Invalidate(address);

                //we need to invalidate in L2 to maintain inclusion
                L2_cache_[0].Invalidate(address);
            }
            break;
        }
    }

    if(found_in_other_L1) {
        cost_ += OTHER_L1_FETCH;
        //Load from L2
        caches_[processor_id].Load(address);
        return;
    }

    // Check for presence in L2 cache
    if(L2_cache_[0].IsPresent(address)) {
        caches_[processor_id].Load(address);
        cost_+= L2_FETCH;
        return;
    }
    // Load from DRAM
    L2_cache_[0].Load(address);
    caches_[processor_id].Load(address);
    cost_ += DRAM_FETCH;
}

void MSICoherence::Store(uint64_t processor_id, uint64_t address) {

    // Check for presence in local L1
    // If the cache line is shared, invalidate all cache lines in L1 and L2
    // Else if the local cache is the only holder upgrade to modified state

    if(caches_[processor_id].IsPresent(address)) {
        // Check for presence in other L1 caches
        bool invalidate_other_L1 = false;

        // Check for presence in the local cache
        for (uint64_t i = 0; i < num_processors_; ++i) {
            if (i != processor_id && caches_[i].IsPresent(address)) {
                caches_[i].Invalidate(address); // Invalidate in L1
                invalidate_other_L1 = true;
            }
        }
        // Local cache was the only sharer
        if(!invalidate_other_L1) {
            cost_ += LOCAL_L1_FETCH;
            caches_[processor_id].Store(address);
            return;
        }
        // Need to fetch from L2
        cost_ += L2_FETCH;
        L2_cache_[0].Store(address);
        caches_[processor_id].Store(address);
        return;
    } else {
        // Check for presence in other L1 caches and invalidate them
        for (uint64_t i = 0; i < num_processors_; ++i) {
            if (i != processor_id && caches_[i].IsPresent(address)) {
                caches_[i].Invalidate(address); // Invalidate in L1
            }
        }
    }

    if(L2_cache_[0].IsPresent(address)) {
        cost_ += L2_FETCH;
        L2_cache_[0].Store(address);
        caches_[processor_id].Store(address);
        return;
    }

    // Fetch from DRAM
    L2_cache_[0].Store(address);
    caches_[processor_id].Store(address);
    cost_ += DRAM_FETCH;
}

uint64_t MSICoherence::GetCost() {
    return cost_;
}