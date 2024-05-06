#include <stdint.h>
#include <vector>
#include <chrono>

#include "CacheBase.h"

#ifndef MSI_CACHE_H
#define MSI_CACHE_H

class MSICacheLine {
public:
    bool M;
    bool S;
    bool V;
    uint64_t tag;
    std::chrono::system_clock::time_point last_accessed;

};

class MSICache : CacheBase {
public:
    MSICache(int num_lines, uint64_t E, uint64_t s, uint64_t b): 
    s_(s),  b_(b),  E_(E) {
        cache_.resize(num_lines);
        for(uint64_t i = 0; i < num_lines; i++) {
            cache_[i] = std::vector<MSICacheLine>(E);
        }
    }

    /* MSI cache does not require initialization */
    void Init();

    /* Check if the address is present in the cache */
    bool IsPresent(uint64_t address);

    /* Send an invalidation request to the cache and return true if the line was invalidated */
    void Invalidate(uint64_t address);

    void Update(uint64_t address, bool M, bool S, bool V);

    bool IsModified(uint64_t address);

    void Load(uint64_t address);

    void Store(uint64_t address);

    virtual ~MSICache() {}

private:
    std::vector<std::vector<MSICacheLine>> cache_;
    int s_;
    int b_;
    int E_;
};

#endif // MSI_CACHE_H