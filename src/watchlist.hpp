#ifndef __WATCHLIST_H_INCLUDED__
#define __WATCHLIST_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace WatchList {
    bool allocate(long*& clarray, int& clused, int& clmax);
    bool allocate(watchlist& wl);
    bool reallocate(long*& clarray, int& clused, int& clmax);
    void deallocate(watchlist& wl);

    bool addWatch(watchlist &wl, int literal, long offset);
    void removeWatch(watchlist &wl, int literal, long* watch);
    void findAndRemoveWatch(watchlist& wl, int literal, long offset);
    bool addConflict(watchlist &wl, int* ptr, long offset);
    void removeConflict(watchlist &wl, int* ptr, long* watch);
    bool isConflict(watchlist &wl);

    bool setWatches(watchlist& wl, model& m, int* ptr, long offset);
    bool processWatches(watchlist& wl, model& m, int* ptr, long offset, int literal, long*& watch, bool soft, bool& rup);
    bool reviseWatches(watchlist& wl, model& m, int* ptr, long offset, int literal, long*& watch, bool& skip);
    bool reviseConflict(watchlist* wl, model& m, int* ptr, long offset, long*& watch);
    bool processList(watchlist &wl, model& m, database& d, int literal, bool soft);
}

#endif
