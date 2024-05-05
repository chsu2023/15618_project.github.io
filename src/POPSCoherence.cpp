#include "POPSCoherence.h"

void POPSCoherence::Load(uint64_t processor_id, uint64_t address) {

    // Check for presence in the local cache
    std::vector<bool> contains_address(num_processors_, false);
    if(caches_[processor_id].IsPresent(address)) {
        // No need to check the state
        cost_ += LOCAL_L1_FETCH;
        bool is_exclusive = true;

        // Cost for snooping
        cost_ += BLOOM_FILTER_LOOKUP;

        for(uint64_t i = 0; i < num_processors_; i++) {
            if(i != processor_id && bloom_filters_[i].contains(address)) {
                is_exclusive = false;
                contains_address[i] = true;
            }
        }
        if(is_exclusive) {
            caches_[processor_id].Update(address, false, true, false, true);
        }
        bloom_filters_[processor_id].insert(address);
        return;
    }

    
    // Check for presence in other L1 caches
    bool found_in_other_L1 = false;   
    for(uint64_t i = 0; i < num_processors_; i++) {
        if(contains_address[i] && caches_[i].IsPresent(address)) {
            cost_ += OTHER_L1_FETCH;
            found_in_other_L1 = true;
            // Check the state
            if(caches_[i].IsModified(address)) {
                caches_[i].Invalidate(address);

                //we need to invalidate in L2 to maintain inclusion
                L2_cache_[0].Invalidate(address);
            }
            else if(caches_[i].IsExclusive(address)) {
                //E->S
                caches_[i].Update(address, false, false, true, true);
            }
            break;
        }
    }


    if(found_in_other_L1) {
        //Load from L2
        bloom_filters_[processor_id].insert(address);
        caches_[processor_id].Load(address);
        return;
    }


    // Check for presence in L2 cache
    if(L2_cache_[0].IsPresent(address)) {
        bloom_filters_[processor_id].insert(address);
        caches_[processor_id].Load(address);
        cost_+= L2_FETCH;
    }
    else{
        // Load from DRAM
        L2_cache_[0].Load(address);
        caches_[processor_id].Load(address);
        bloom_filters_[processor_id].insert(address);
        cost_ += DRAM_FETCH;
    }
}

void POPSCoherence::Store(uint64_t processor_id, uint64_t address) {
    bool invalidata_other_L1 = false;
    bool invalidate_other_L2 = false;

    std::vector<bool> contains_address(num_processors_, false);
    cost_ += BLOOM_FILTER_LOOKUP;

    // Check for presence in the local cache
    // Combining the loop for bloom filter lookup and invalidation
    for (uint64_t i = 0; i < num_processors_; ++i) {
        if (i != processor_id && caches_[i].IsPresent(address)) {
            invalidata_other_L1 = true;
            invalidate_other_L2 |= caches_[i].IsModified(address);
            caches_[i].Invalidate(address); // Invalidate in L1 
            contains_address[i] = true;
            cost_ += OTHER_L1_FETCH;
        }
    }

    if (invalidata_other_L1 && invalidate_other_L2) {
        L2_cache_[0].Invalidate(address); // Invalidate in L2 to maintain inclusiveness
    }

    if(caches_[processor_id].IsPresent(address)) {
        cost_ += LOCAL_L1_FETCH;
        return;
    }

    caches_[processor_id].Store(address);
    // Update in L2 to maintain inclusiveness
    L2_cache_[0].Store(address); 
    cost_ += L2_FETCH;

}

uint64_t POPSCoherence::GetCost() {
    return cost_;
}