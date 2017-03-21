#ifndef __WATCHLIST_H_INCLUDED__
#define __WATCHLIST_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace WatchList {
    bool allocate(long*& clarray, int& clused, int& clmax);
    bool allocate(watchlist& wl);
    bool reallocate(long*& clarray, int& clused, int& clmax);
    void deallocate(watchlist& wl);

    bool WatchList::addWatches(watchlist& wl, model& m, int*& ptr, long offset, bool& trigger);
    bool WatchList::addToList(watchlist &wl, int literal, long offset);
    bool WatchList::processList(watchlist &wl, model& m, database& d, int literal);


    bool insertWatches(watchlist& wl, proofiterator& i);
    bool removeWatches(watchlist& wl, proofiterator& i);
    bool insertClause(clauselist &cl, long offset);
    bool removeClause(clauselist &cl, long offset);
}

#endif
