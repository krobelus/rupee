#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"
#include "model.hpp"
#include "database.hpp"
#include "watchlist.hpp"
#include "revision.hpp"

namespace Revision {

int* stackleft;
int* stackright;
long reason;
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

void deallocate(revision& v) {
    Blablabla::log("Deallocating revision queue.");
    v.lits -= Parameters::noVariables;
    free(v.array);
    free(v.lits);
}

void addLiteral(revision& v, model& m, database& d, int lit) {
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
    while(!Model::isSatisfied(m, *ptr)) {
        ++ptr;
    }
    literal = *ptr;
    prev[Constants::EndOfModel] = m.last;
    addLiteral(v, m, d, literal);
    literal = m.next[literal];
    while(literal != Constants::EndOfModel) {
        if(Revision::isInCone(v, m.reason[literal], literal, d)) {
            addLiteral(v, m, d, literal);
            literal = m.next[literal];
            Model::dropLiteral(m, d, literal);
        } else {
            literal = m.next[literal];
        }
    }
    m.forced = m.last = prev[Constants::EndOfModel];
    m.head = Constants::EndOfModel;
    Blablabla::log("Revision list:");
    Blablabla::logRevision(v);
    Blablabla::log("Current model:");
    Blablabla::logModel(m);
}

bool reviseList(revision& v, watchlist& wl, model& m, database& d) {
    coneit = v.array;
    while(coneit < v.used) {
        skip = false;
        literal = *(coneit++);
        Blablabla::log("Revising literal " + Blablabla::litToString(literal) + ".");
        Blablabla::increase();
        Blablabla::logWatchList(wl, literal, d);
        watchit = wl.array[literal];
        while((cloffset = *watchit) != Constants::EndOfWatchList && !skip) {
            clause = Database::getPointer(d, cloffset);
            Blablabla::log("Revising clause " + Blablabla::clauseToString(clause));
            Blablabla::increase();
            if(!WatchList::reviseWatches(wl, m, clause, cloffset, literal, watchit, skip)) { return false; }
            Blablabla::decrease();
        }
        Blablabla::decrease();
        v.lits[literal] = false;
    }
    v.used = v.array;
    Blablabla::log("Revising conflict list.");
    Blablabla::increase();
    Blablabla::logWatchList(wl, Constants::ConflictWatchlist, d);
    watchit = wl.array[Constants::ConflictWatchlist];
    while((cloffset = *watchit) != Constants::EndOfWatchList) {
        clause = Database::getPointer(d, cloffset);
        Blablabla::log("Revising clause " + Blablabla::clauseToString(clause));
        Blablabla::increase();
        if(!WatchList::reviseConflict(wl, m, clause, cloffset, watchit)) { return false; }
        Blablabla::decrease();
    }
    Blablabla::decrease();
    return true;
}

}
