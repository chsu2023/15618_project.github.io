#include <stdint.h>
#include <vector>
#include <chrono>

#include "CacheBase.h"

#ifndef POPS_CACHE_H
#define POPS_CACHE_H


class POPSCacheLine {
public:
    bool M;
    bool E;
    bool S;
    bool V;
    uint64_t tag;
    std::chrono::system_clock::time_point last_accessed;
};

class POPSCache : CacheBase {
public:
    POPSCache(int num_lines, uint64_t E, uint64_t s, uint64_t b):
    num_lines_(num_lines), s_(s), b_(b), E_(E) {
        cache_.resize(num_lines);
        for(uint64_t i = 0; i < num_lines; i++) {
            cache_[i] = std::vector<POPSCacheLine>(E);
        }
        lines_used_ = 0;
    }
    void Init();

    bool IsPresent(uint64_t source);

    bool IsPresentInOther(uint64_t source);

    void Invalidate(uint64_t address);

    void Update(uint64_t address, bool M, bool E, bool S, bool V);

    bool IsModified(uint64_t address);

    void Load(uint64_t address);

    void Store(uint64_t address);

    bool IsExclusive(uint64_t address);

    virtual ~POPSCache() {}
private:
    std::vector<std::vector<POPSCacheLine>> cache_;
    int num_lines_;
    int lines_used_;
    int s_;
    int b_;
    int E_;
};

#endif // POPS_CACHE_H