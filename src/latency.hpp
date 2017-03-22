#ifndef __LATENCY_H_INCLUDED__
#define __LATENCY_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace Latency {
    bool allocate(latency& x);
    bool reallocate(latency& x);
    void deallocate(latency& x);
    void addLiteral(latency& x, int literal);
    bool addClause(latency& x, long offset);
    bool findResolvableClauses(latency &x, watchlist &wl, database &d);
    void saveOriginal(latency& x);
    void restore(latency& x);
    void unload(latency& x);
    void loadClause(latency& x, int* ptr, int pivot);
    bool computeResolvent(latency& x, int* ptr);
}

#endif
