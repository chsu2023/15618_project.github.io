#include "MESIFCoherence.h"

void MESIFCoherence::Load(int64_t address, int64_t processor_id) {

    // Check for presence in the local cache
    if(caches_[processor_id].IsPresent(address)) {
        // No need to check the state
        cost_ += LOCAL_L1_FETCH;
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
                caches_[num_processors_].Invalidate(address);
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
    if(caches_[num_processors_].IsPresent(address)) {
        caches_[processor_id].Load(address);
        cost_+= L2_FETCH;
    }
    else{
        // Load from DRAM
        caches_[num_processors_].Load(address);
        caches_[processor_id].Load(address);
        cost_ += DRAM_FETCH;
    }

    
}

void MESIFCoherence::Store(int64_t address, int64_t processor_id) {
    bool invalidata_other_L1 = false;

    // Check for presence in the local cache
    for (int64_t i = 0; i < num_processors_; ++i) {
        if (i != processor_id && caches_[i].IsPresent(address)) {
            caches_[i].Invalidate(address); // Invalidate in L1
            invalidata_other_L1 = true;
        }
    }

    if (invalidata_other_L1) {
        caches_[num_processors_].Invalidate(address); // Invalidate in L2 to maintain inclusiveness
    }

    caches_[processor_id].Store(address);
    // Update in L2 to maintain inclusiveness
    caches_[num_processors_].Store(address); 
    cost_ += LOCAL_L1_FETCH;

}

int64_t MESIFCoherence::GetCost() {
    return cost_;
}