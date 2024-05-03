#include "MESIFCache.h"

void MESIFCache::Init() {

}
bool MESIFCache::IsPresent(int64_t address) {
    // Extract the set for the address address
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));

    // Check the tags for the extracted set
    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag && cache_line.V) {
            return true;
        }
    }
    return false;
}

void MESIFCache::Update(int64_t address, bool M, bool E, bool S, bool V, bool F) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));

    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            cache_line.M = M;
            cache_line.S = S;
            cache_line.V = V;
            cache_line.E = E;
            cache_line.F = F;
            return;
        }
    }

}

void MESIFCache::Invalidate(int64_t address) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));

    auto cache_lines = cache_[address_set];
    for ( auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            cache_line.V = false;
        }
    }
}

bool MESIFCache::IsModified(int64_t address) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));
    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            return cache_line.M;
        }
    }
    return false;
}

bool MESIFCache::IsExclusive(int64_t address) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));
    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            return cache_line.E;
        }
    }
    return false;
}

bool MESIFCache::IsForward(int64_t address) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));
    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            return cache_line.F;
        }
    }
    return false;
}

void MESIFCache::Load(int64_t address) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));
    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            //The line is already present in the cache
            if(!cache_line.V){
                //I->E transion
                cache_line.V = true;
                cache_line.E = true;
                cache_line.S = false;
                cache_line.M = false;
            }
            cache_line.last_accessed = std::chrono::system_clock::now();

            return;
        }
    }

    //I->E
    MESIFCacheLine new_line;
    new_line.tag = address_tag;
    new_line.V = true;
    new_line.E = true;
    new_line.S = false;
    new_line.M = false;
    new_line.F = false;
    new_line.last_accessed = std::chrono::system_clock::now();

    // Check if there is space in the cache
    if(lines_used_ < E_) {
        cache_[address_set].push_back(new_line);
        lines_used_++;
    }
    else {
        // Evict the least recently used line
        auto min_time = std::chrono::system_clock::now();
        int min_index = 0;
        for(int i = 0; i < E_; i++) {
            if(cache_[address_set][i].last_accessed < min_time) {
                min_time = cache_[address_set][i].last_accessed;
                min_index = i;
            }
        }
        cache_[address_set][min_index] = new_line;
    }
}

void MESIFCache::Store(int64_t address) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));
    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            //The line is already present in the cache
            //X->M
            cache_line.M = true;
            cache_line.V = true;
            cache_line.S = false;
            cache_line.E = false;
            cache_line.F = false;
            cache_line.last_accessed = std::chrono::system_clock::now();
            return;
        }
    }

    //X->M
    MESIFCacheLine new_line;
    new_line.tag = address_tag;
    new_line.V = true;
    new_line.S = false;
    new_line.M = true;
    new_line.E = false;
    new_line.F = false;
    new_line.last_accessed = std::chrono::system_clock::now();

    // Check if there is space in the cache
    if(lines_used_ < E_) {
        cache_[address_set].push_back(new_line);
        lines_used_++;
    }
    else {
        // Evict the least recently used line
        auto min_time = std::chrono::system_clock::now();
        int min_index = 0;
        for(int i = 0; i < E_; i++) {
            if(cache_[address_set][i].last_accessed < min_time) {
                min_time = cache_[address_set][i].last_accessed;
                min_index = i;
            }
        }
        cache_[address_set][min_index] = new_line;
    }
}


// void MESIFCache::BusRD_Response(int64_t address) {
//     int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
//     int address_tag = (address >> (b_ + s_));
//     auto cache_lines = cache_[address_set];

//     for(auto & cache_line : cache_lines) {
//         if(cache_line.tag == address_tag) {
//             cache_line.V = true;
//             return;
//         }
//     }
// }