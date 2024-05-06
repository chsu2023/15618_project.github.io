#include "MESIFCoherence.h"

void MESIFCoherence::Load(uint64_t processor_id, uint64_t address) {

    // Check for presence in the local cache
    if(caches_[processor_id].IsPresent(address)) {
        // No need to check the state
        cost_ += LOCAL_L1_FETCH;
        bool is_exclusive = true;

        // Cost for snooping

        for(uint64_t i = 0; i < num_processors_; i++) {
            cost_ += BROADCAST_LOOKUP;
            if(i != processor_id && caches_[i].IsPresent(address)) {
                is_exclusive = false;
            }
        }
        if(is_exclusive) {
            caches_[processor_id].Update(address, false, true, false, true, true);
        }
        return;
    }

    
    // Check for presence in other L1 caches
    bool found_in_other_L1 = false;    
    for(uint64_t i = 0; i < num_processors_; i++) {
        if(i != processor_id && caches_[i].IsPresent(address)) {
            found_in_other_L1 = true;
            // Check the state
            if(caches_[i].IsModified(address)) {
                caches_[i].Invalidate(address);

                //we need to invalidate in L2 to maintain inclusion
                L2_cache_[0].Invalidate(address);
            }
            else if(caches_[i].IsExclusive(address)) {
                //E->S
                caches_[i].Update(address, false, false, true, true, false);
            }
            else if(caches_[i].IsForward(address)) {
                // Can be left as such because we incur only only one sided cost
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
    }
    else{
        // Load from DRAM
        L2_cache_[0].Load(address);
        caches_[processor_id].Load(address);
        cost_ += DRAM_FETCH;
    }
}

void MESIFCoherence::Store(uint64_t processor_id, uint64_t address) {
    bool invalidata_other_L1 = false;
    bool invalidate_other_L2 = false;

    // Check for presence in the local cache
    for (uint64_t i = 0; i < num_processors_; ++i) {
        cost_ += BROADCAST_LOOKUP;
        if (i != processor_id && caches_[i].IsPresent(address)) {
            invalidata_other_L1 = true;
            invalidate_other_L2 |= caches_[i].IsModified(address);
            caches_[i].Invalidate(address); // Invalidate in L1 
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

uint64_t MESIFCoherence::GetCost() {
    return cost_;
}