#include <stdlib.h>

#include "structs.hpp"
#include "model.hpp"
#include "database.hpp"
#include "watchlist.hpp"
#include "revision.hpp"

namespace Revision {

int* stackleft;
int* stackright;
int* reason;
int it;
int literal;
int* clause;
int conelit;
int* coneit;
long* watchit;
long cloffset;
bool skip;

bool allocate(revision& v) {
    Blablabla::log("Allocating revision queue.");
    v.array = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    v.lits = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    v.used = v.array;
    if(v.array == NULL || v.lits == NULL) {
        Blablabla::log("Error at revision queue allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    } else {
        v.lits += Parameters::noVariables;
        for(it = -Parameters::noVariables; it <= Parameters::noVariables; ++it) {
            v.lits[it] = false;
        }
        return true;
    }
}

bool deallocate(revision& v) {
    Blablabla::log("Deallocating revision queue.");
    free(v.array);
}

void addLiteral(int lit) {
    v.lits[literal] = true;
    *(v.used++) = lit;
}

bool isInCone(revision& v, long offset, int lit, database& d) {
    clause = Database::getPointer(d, offset);
    while((conelit = *clause) != Constants::EndOfClause) {
        if(conelit != lit && v.lits[-conelit]) {
            return true;
        }
        ++clause;
    }
    return false;
}

void getCone(revision& v, model& m, database& d, int* ptr) {
    Blablabla::logReasons(m, d);
    while(!Model::isSatisfied(*ptr)) {
        ++ptr;
    }
    literal = *ptr;
    addLiteral(literal);
    stackright = m.positions[literal];
    stackleft = stackright++;
    while(stackright < m.used) {
        literal = *stackright;
        reason = m.reasons[literal];
        if(Revision::isInCone(v, reason, literal, d)) {
            addLiteral(literal);
            m.satisfied[literal] = false;
        } else {
            *stackleft = *stackright;
            m.positions[literal] = stackleft++;
        }
    }
    m.forced = m.head = m.used = stackLeft;
    Blablabla::log("Revision list:");
    Blablabla::logRevision(v);
    Blablabla::log("Current model:");
    Blablabla::logModel(m);
}

bool reviseList(revision& v, watchlist& wl, model& m, database& d) {
    coneit = v.array;
    while(coneit < v.used) {
        skip = false;
        literal = *(++coneit);
        Blablabla::log("Revising literal " + Blablabla::litToString(literal) + ".");
        Blablabla::increase();
        Blablabla::logWatchList(wl, literal);
        watchit = wl.array[literal];
        while((cloffset = *watchit) != Constants::EndOfWatchList && !skip) {
            clause = Database::getPointer(d, cloffset);
            if(!WatchList::reviseWatches(wl, m, clause, cloffset, literal, watchit, skip)) { return false; }
        }
        Blablabla::decrease();
        v.lits[literal] = false;
    }
    Blablabla::log("Revising conflict list.");
    Blablabla::increase();
    Blablabla::logWatchlist(wl, Constants::ConflictWatchlist);
    watchit = wl.array[Constants::ConflictWatchlist];
    while((cloffset = *watchit) != Constants::EndOfWatchList) {
        clause = Database::getPointer(d, cloffset);
        if(!WatchList::reviseConflicts(wl, m, clause, cloffset, literal, watchit)) { return false; }
    }
    Blablabla::decrease();
    return true;
}

}
