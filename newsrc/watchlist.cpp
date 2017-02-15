#include <stdlib.h>

#include "extra.hpp"
#include "watchlist.hpp"

namespace WatchList {

bool error;
int it;

bool allocate(clauselist& cl) {
    cl.array = (long*) malloc(Parameters::hashDepth * sizeof(long));
    cl.used = 0;
    cl.max = Parameters::hashDepth;
    return cl.array != NULL;
}

bool allocate(watchlist& wl) {
    error = false;
    Blablabla::log("Allocating watchlist.");
    wl.longlist = (struct clauselist*) malloc((2 * Parameters::noVariables + 1) * sizeof(struct clauselist));
    wl.unitlist = (struct clauselist*) malloc((2 * Parameters::noVariables + 1) * sizeof(struct clauselist));
    if(wl.longlist == NULL || wl.unitlist == NULL) {
        error = true;
    } else {
        wl.longlist += Parameters::noVariables;
        wl.unitlist += Parameters::noVariables;
        for(it = -Parameters::noVariables; !error && it <= Parameters::noVariables; ++it) {
            if(!allocate(wl.longlist[it]) || !allocate(wl.unitlist[it])) {
                error = true;
            }
        }
        if(!error && !allocate(wl.conflictlist)) {
            error = true;
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

bool reallocate(clauselist& cl) {
    Blablabla::log("Reallocating watchlist.");
    cl.max = (cl.max * 3) << 1;
    cl.array = (long*) realloc(cl.array, cl.max * sizeof(long));
    if(cl.array == NULL) {
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
        free(wl.longlist[it].array);
        free(wl.unitlist[it].array);
    }
    free(wl.longlist);
    free(wl.unitlist);
    free(wl.conflictlist.array);
}

bool setInitialWatches(watchlist& wl, proofiterator& i) {
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

bool insertClause(clauselist &cl, long offset) {
    if(cl.used >= cl.max) {
        if(!WatchList::reallocate(cl)) { return false; }
    }
    cl.array[cl.used++] = offset;
    return true;
}

}
