#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"
#include "database.hpp"
#include "watchlist.hpp"

namespace WatchList {

bool error;
int it;
int removeit;

bool allocate(long*& clarray, int& clused, int& clmax) {
    clarray = (long*) malloc(Parameters::hashDepth * sizeof(long));
    clused = 0;
    clmax = Parameters::hashDepth;
    return clarray != NULL;
}

bool allocate(watchlist& wl) {
    error = false;
    Blablabla::log("Allocating watchlist.");
    wl.array = (long**) malloc((2 * Parameters::noVariables + 1) * sizeof(long*));
    wl.used = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(long*));
    wl.max = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(long*));
    if(wl.array == NULL || wl.used == NULL || wl.max == NULL) {
        error = true;
    } else {
        wl.array += Parameters::noVariables;
        wl.used += Parameters::noVariables;
        wl.max += Parameters::noVariables;
        for(it = -Parameters::noVariables; !error && it <= Parameters::noVariables; ++it) {
            if(!allocate(wl.array[it], wl.used[it], wl.max[it])) {
                error = true;
            }
        }
    }
    if(error) {
        Blablabla::log("Error at watchlist allocation.");
        Blablabla::comment("Memory management error.");
        return false;
    } else {
        return true;
    }
}

bool reallocate(long*& clarray, int& clused, int& clmax) {
    Blablabla::log("Reallocating watchlist.");
    clmax = (clmax * 3) << 1;
    clarray = (long*) realloc(clarray, clmax * sizeof(long));
    if(clarray == NULL) {
        Blablabla::log("Error at watchlist allocation.");
        Blablabla::comment("Memory management error.");
        return false;
    } else {
        return true;
    }
}

void deallocate(watchlist& wl) {
    Blablabla::log("Deallocating watchlist.");
    for(it = -Parameters::noVariables; it <= Parameters::noVariables; ++it) {
        free(wl.array[it]);
    }
    wl.array -= Parameters::noVariables;
    wl.used -= Parameters::noVariables;
    wl.max -= Parameters::noVariables;
    free(wl.array);
    free(wl.used);
    free(wl.max);
}

bool insertWatches(watchlist& wl, trail& t, long offset, int* pointer) {

}




bool insertWatches(watchlist& wl, long offset, int* pointer) {
    switch(Database::classifyClause(i.pointer)) {
    case Constants::ClauseSizeConflict :
        if(!WatchList::insertClause(wl.conflictlist, i.offset)) { return false; }
        break;
    case Constants::ClauseSizeUnit :
        if(!WatchList::insertClause(wl.unitlist[*(i.pointer)], i.offset)) { return false; }
        break;
    case Constants::ClauseSizeLong :
        if(!WatchList::insertClause(wl.longlist[i.pointer[0]], i.offset)) { return false; }
        if(!WatchList::insertClause(wl.longlist[i.pointer[1]], i.offset)) { return false; }
        break;
    }
    return true;
}

bool removeWatches(watchlist& wl, long offset, int* pointer) {
    switch(Database::classifyClause(i.pointer)) {
    case Constants::ClauseSizeConflict :
        if(!WatchList::removeClause(wl.conflictlist, i.offset)) { return false; }
        break;
    case Constants::ClauseSizeUnit :
        if(!WatchList::removeClause(wl.unitlist[*(i.pointer)], i.offset)) { return false; }
        break;
    case Constants::ClauseSizeLong :
        if(!WatchList::removeClause(wl.longlist[i.pointer[0]], i.offset)) { return false; }
        if(!WatchList::removeClause(wl.longlist[i.pointer[1]], i.offset)) { return false; }
        break;
    }
    return true;
}

bool insertClause(clauselist &cl, long offset) {
    if(cl.used >= cl.max) {
        if(!WatchList::reallocate(cl)) { return false; }
    }
    cl.array[cl.used++] = offset;
    return true;
}

bool removeClause(clauselist &cl, long offset) {
    for(removeit = 0; removeit < cl.used; ++removeit) {
        if(cl.array[removeit] == offset) {
            cl.array[removeit] = cl.array[--cl.used];
            return true;
        }
    }
    return false;
}

}
