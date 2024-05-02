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
    int64_t tag;
    std::chrono::system_clock::time_point last_accessed;

};

class MSICache : CacheBase {
public:
    explicit MSICache(int num_lines, int64_t E, int64_t s, int64_t b): 
    num_lines_(num_lines), s_(s),  b_(b),  E_(E) {
        cache_.resize(num_lines);
        for(int64_t i = 0; i < num_lines; i++) {
            cache_[i] = std::vector<MSICacheLine>(E);
        }
        lines_used_ = 0;
    }

    /* MSI cache does not require initialization */
    void Init();

    /* Check if the address is present in the cache */
    bool IsPresent(int64_t address);

    /* Send an invalidation request to the cache and return true if the line was invalidated */
    void Invalidate(int64_t address);

    void Update(int64_t address, bool M, bool S, bool V);

    bool IsModified(int64_t address);

    void Load(int64_t address);

    void Store(int64_t address);

private:
    std::vector<std::vector<MSICacheLine>> cache_;
    int num_lines_;
    int lines_used_;
    int s_;
    int b_;
    int E_;
};

#endif // MSI_CACHE_H