#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"
#include "database.hpp"
#include "model.hpp"
#include "watchlist.hpp"

namespace WatchList {

bool error;
int it;
int removeit;
int* candidate;
int clliteral;
long* watchit;
long offwatch;
int othlit;

bool allocate(long*& clarray, int& clused, int& clmax) {
    clarray = (long*) malloc(Parameters::hashDepth * sizeof(long));
    clused = 0;
    clmax = Parameters::hashDepth;
    if(clarray == NULL) {
        return false;
    } else {
        clarray[0] = 0;
        return true;
    }
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
    clmax = clmax * 4;
    clarray = (long*) realloc(clarray, clmax * sizeof(long));
    if(clarray == NULL) {
        Blablabla::log("Error at watchlist allocation.");
        Blablabla::comment("Memory management error.");
        return false;
    }
    return true;
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

bool addWatch(watchlist &wl, int literal, long offset) {
    if(wl.used[literal] >= wl.max[literal]) {
        if(!reallocate(wl.array[literal], wl.used[literal], wl.max[literal])) { return false; }
    }
    wl.array[literal][wl.used[literal]++] = offset;
    wl.array[literal][wl.used[literal]] = Constants::EndOfWatchList;
    return true;
}

void removeWatch(watchlist &wl, int literal, long* watch) {
    *watch = wl.array[literal][--wl.used[literal]];
    wl.array[literal][wl.used[literal]] = Constants::EndOfWatchList;
}

void findAndRemoveWatch(watchlist& wl, int literal, long offset) {
    watchit = wl.array[literal];
    while((offwatch = *watchit) != Constants::EndOfWatchList) {
        if(offwatch == offset) {
            removeWatch(wl, literal, watchit);
            return;
        } else {
            ++watchit;
        }
    }
}

bool addConflict(watchlist &wl, int* ptr, long offset) {
    if(Database::isFlag(ptr, Constants::ConflictBit, Constants::SatisfiableFlag)) {
        Blablabla::log("Adding to conflict list.");
        Database::setFlag(ptr, Constants::ConflictBit, Constants::ConflictFlag);
        return addWatch(wl, Constants::ConflictWatchlist, offset);
    } else {
        Blablabla::log("Already in conflict list.");
        return true;
    }
}

void removeConflict(watchlist &wl, int* ptr, long* watch) {
    Blablabla::log("Removing from conflict list.");
    Database::setFlag(ptr, Constants::ConflictBit, Constants::SatisfiableFlag);
    return removeWatch(wl, Constants::ConflictWatchlist, watch);
}

bool isConflict(watchlist &wl) {
    return wl.used[Constants::ConflictWatchlist] != 0;
}

bool setWatches(watchlist& wl, model& m, int* ptr, long offset) {
    candidate = ptr;
    if(Model::findNextNonFalsified(m, candidate, triglit)) {
        trigger = false;
        Blablabla::log("Falsified clause.");
        return WatchList::addConflict(wl, ptr, offset);
    } else {
        *candidate = ptr[0];
        ptr[0] = triglit;
        if(!WatchList::addWatch(wl, triglit, offset)) { return false; }
        if(Model::isSatisfied(triglit)) {
            Blablabla::log("Satisfied clause.");
            Blablabla::log("Setting watches as " + Blablabla::clauseToString(ptr) + ".");
            trigger = false;
        } else if(!(trigger = Model::findNextNonFalsified(m, ++candidate, clliteral))) {
            *candidate = ptr[1];
            ptr[1] = clliteral;
            Blablabla::log("Pseudoidle clause.");
            Blablabla::log("Setting watches as " + Blablabla::clauseToString(ptr) + ".");
        } else {
            Blablabla::log("Triggered clause.");
            Blablabla::log("Setting watches as " + Blablabla::clauseToString(ptr) + ".");
            Model::propagateLiteral(c.stack, triglit, c.offset, c.pointer);
        }
        return WatchList::addWatch(wl, ptr[1], offset);
    }
}

bool processWatches(watchlist& wl, model& m, int* ptr, long offset, int literal, long*& watch, bool soft, bool& rup) {
    if(m.isSatisfied(ptr[0]) || m.isSatisfied(ptr[1])) {
        Blablabla::log("Satisfied clause.");
        ++watch;
        return true;
    } else {
        if(ptr[0] == literal) {
            ptr[0] = ptr[1];
        }
        candidate = ptr + 2;
        if(Model::findNextNonFalsified(m, candidate, clliteral)) {
            candidate = literal;
            ptr[1] = clliteral;
            Blablabla::log("Replacing watches as" + Blablabla::clauseToString(ptr) + ".");
            removeWatch(wl, literal, watch);
            return addWatch(wl, literal, offset);
        } else {
            ptr[1] = literal;
            clliteral = ptr[0];
            if(m.isFalsified(clliteral)) {
                Blablabla::log("Falsified clause.");
                if(soft == Constants::SoftPropagation) {
                    Blablabla::log("Soft conflict detected.");
                    m.reasons[Constants::ConflictWatchlist] = offset;
                    rup = true;
                } else {
                    removeWatch(wl, literal, offset);
                    findAndRemoveWatch(wl, clliteral, offset);
                    addConflict(wl, ptr, offset);
                }
                ++watch;
            } else {
                Blablabla::log("Triggered clause.");
                Model::propagateLiteral(m, clliteral, offset, ptr);
                ++watch;
            }
            return true;
        }
    }
}

bool reviseWatches(watchlist& wl, model& m, int* ptr, long offset, int literal, long*& watch, bool& skip) {
    if(ptr[0] == literal) {
        othlit = ptr[1];
    } else {
        othlit = ptr[0];
        ptr[0] = literal;
    }
    if(!Model::isFalsified(m,othlit)) {
        Blablabla::log("Watches are correct.");
    } else {
        candidate = ptr + 2;
        if(Model::findNextNonFalsified(m, candidate, clliteral)) {
            Blablabla::log("Triggered clause.");
            Model::propagateLiteral(m, literal, offset, ptr);
            skip = true;
        } else {
            *candidate = othlit;
            ptr[1] = clliteral;
            WatchList::findAndRemoveWatch(wl, othlit, offset);
            if(!addWatch(wl, clliteral, offset)) { return false; }
            Blablabla::log("Replacing watches as" + Blablabla::clauseToString(ptr) + ".");
        }
    }
    ++watch;
    return true;
}

bool reviseConflict(watchlist* wl, model& m, int* ptr, long offset, long*& watch) {
    if(Model::findNextNonFalsified(m, ptr, clliteral)) {
        ++watch;
        return true;
    } else {
        removeConflict(wl, ptr, watch);
        setWatches(wl, m, ptr, offset);
    }
}

bool processList(watchlist &wl, model& m, database& d, int literal, bool soft) {
    Blablabla::increase();
    Blablabla::logWatchList(wl, literal, d);
    watchit = wl.array[literal];
    while((cloffset = *watchit) != Constants::EndOfWatchList && !(m.softConflict)) {
        clptr = Database::getPointer(d, cloffset);
        Blablabla::log("Processing clause " + Blablabla::clauseToString(clptr));
        Blablabla::increase();
        if(!processWatches(wl, m, clptr, literal, watchit, soft)) { return false; }
        Blablabla::decrease();
    }
    Blablabla::decrease();
    return true;
}

}
