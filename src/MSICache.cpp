#include "MSICache.h"

void MSICache::Init() {

}
bool MSICache::IsPresent(int64_t address) {
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

void MSICache::Update(int64_t address, bool M, bool S, bool V) {
    int address_set = (address >> b_) & ((1 << (s_ + 1)) - 1);
    int address_tag = (address >> (b_ + s_));

    auto cache_lines = cache_[address_set];

    for(auto & cache_line : cache_lines) {
        if(cache_line.tag == address_tag) {
            return;
        }
    }

}

bool MSICache::Invalidate(int64_t address) {
    return false;
}

