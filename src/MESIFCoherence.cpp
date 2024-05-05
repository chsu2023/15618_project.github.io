#include "MESIFCoherence.h"

void MESIFCoherence::Load(int64_t processor_id, int64_t address) {

    // Check for presence in the local cache
    if(caches_[processor_id].IsPresent(address)) {
        // No need to check the state
        cost_ += LOCAL_L1_FETCH;
        bool is_exclusive = true;

        for(int64_t i = 0; i < num_processors_; i++) {
            if(i != processor_id && caches_[i].IsPresent(address)) {
                is_exclusive = false;
                break;
            }
        }
        if(is_exclusive) {
            caches_[processor_id].Update(address, false, true, false, true, true);
        }
        return;
    }

    
    // Check for presence in other L1 caches
    bool found_in_other_L1 = false;
    bool shared = false;    
    for(int64_t i = 0; i < num_processors_; i++) {
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
                shared = true;
            }
            else if(caches_[i].IsForward(address)) {
                //TODO: Implement the forward state transitionS
                shared = true;
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

void MESIFCoherence::Store(int64_t processor_id, int64_t address) {
    bool invalidata_other_L1 = false;
    bool invalidate_other_L2 = false;

    // Check for presence in the local cache
    for (int64_t i = 0; i < num_processors_; ++i) {
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
    cost_ += LOCAL_L1_FETCH;

}

int64_t MESIFCoherence::GetCost() {
    return cost_;
}