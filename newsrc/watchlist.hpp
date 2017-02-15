#ifndef __WATCHLIST_H_INCLUDED__
#define __WATCHLIST_H_INCLUDED__

#include <stdlib.h>

struct clauselist{
    long* array;
    int used;
    int max;
};

struct watchlist{
    struct clauselist* longlist;
    struct clauselist* unitlist;
    struct clauselist conflictlist;
};

namespace WatchList {
    bool allocate(clauselist& cl);
    bool allocate(watchlist& wl);
    bool reallocate(clauselist& cl);
    void deallocate(watchlist& wl);

    bool setInitialWatches(watchlist& wl, proofiterator& i);
    bool insertClause(clauselist &cl, long offset);
}

#endif
