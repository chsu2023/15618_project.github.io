#include <stdint.h>
#include <vector>
#include <chrono>

#include "CacheBase.h"

#ifndef MESIF_CACHE_H
#define MESIF_CACHE_H

class MESIFCacheLine {
public:
    bool M;
    bool E;
    bool S;
    bool V;
    bool F;
    uint64_t tag;
    std::chrono::system_clock::time_point last_accessed;

};

class MESIFCache : CacheBase {
public:
    explicit MESIFCache(int num_lines, uint64_t E, uint64_t s, uint64_t b): 
    num_lines_(num_lines), s_(s),  b_(b),  E_(E) {
        cache_.resize(num_lines);
        for(uint64_t i = 0; i < num_lines; i++) {
            cache_[i] = std::vector<MESIFCacheLine>(E);
        }
        lines_used_ = 0;
    }

    /* MESIF cache does not require initialization */   
    void Init();

    /* Check if the address is present in the cache */
    bool IsPresent(uint64_t address);

    /* Send an invalidation request to the cache and return true if the line was invalidated */
    void Invalidate(uint64_t address);

    void Update(uint64_t address, bool M, bool E, bool S, bool V, bool F);

    bool IsModified(uint64_t address);

    void Load(uint64_t address);

    void Store(uint64_t address);

    bool IsExclusive(uint64_t address);

    bool IsForward(uint64_t address);


private:
    std::vector<std::vector<MESIFCacheLine>> cache_;
    int num_lines_;
    int lines_used_;
    int s_;
    int b_;
    int E_;
};

#endif // MESIF_CACHE_H