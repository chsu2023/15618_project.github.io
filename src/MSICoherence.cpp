#include "MSICoherence.h"

void MSICoherence::Load(int64_t address, int64_t processor_id) {

    // Check for presence in the local cache
    if(caches_[processor_id].IsPresent(address)) {
        // No need to check the state
        cost_ += LOCAL_L1_FETCH;
        return;
    }

    // Check for presence in other L1 caches
    for(int64_t i = 0; i < num_processors_; i++) {
        if(i != processor_id && caches_.IsPresent(address)) {
            // Check the state
            if(caches_[i].IsModified(address)) {
                caches_[i].Invalidate(address);
            }
            // Add to L2
            caches_[num_processors_].Store(address);

            // Load to the requesting processors
            caches_[processor_id].Load(address);
            cost_ += OTHER_L1_FETCH;
            return;
        }
    }

    // Check for presence in L2 cache
    if(caches_[num_processors_].IsPresent(address)) {
        caches_[processor_id].Load(address);
        cost_+= L2_FETCH;
        return;
    }

    // Load from DRAM
    caches_[num_processors_].Load(address);
    caches_[processor_id].Load(address);
    cost_ += DRAM_FETCH;
}

void MSICoherence::Store(int64_t address, int64_t processor_id) {
    
}

int64_t MSICoherence::GetCost() {
    return cost_;
}