#include <stdint.h>

#ifndef CACHE_BASE_H
#define CACHE_BASE_H

class CacheBase {

public:

    virtual void Init() = 0;

    virtual bool IsPresent(int64_t source) = 0;

    virtual bool Invalidate(int64_t address) = 0;

};

#endif // CACHE_BASE_H