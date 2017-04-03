#include <stdlib.h>
#include <string>

#include "structs.hpp"
#include "extra.hpp"
#include "watchlist.hpp"
#include "database.hpp"
#include "model.hpp"

namespace Model {

int it;
bool dummyrup;
long reasonoffset;

bool allocate(model& m) {
    Blablabla::log("Allocating model.");
    m.prev = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    m.next = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    m.satisfied = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    m.reason = (long*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    if(m.prev == NULL || m.next == NULL || m.satisfied == NULL || m.reason == NULL) {
        Blablabla::log("Error at model allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    }
    m.prev += Parameters::noVariables;
    m.next += Parameters::noVariables;
    m.satisfied += Parameters::noVariables;
    m.reason += Parameters::noVariables;
    for(it = 0; it <= Parameters::noVariables; ++it) {
        m.satisfied[it] = m.satisfied[-it] = false;
        m.reasons[it] = m.reasons[-it] = 0;
    }
    m.forced = m.head = m.last = Constants::ReservedLiteral;
    m.next[Constants::ReservedLiteral] = m.prev[Constants::ReservedLiteral] = Constants::EndOfModel;
    m.satisfied[Constants::ReservedLiteral] = true;
    m.reason[Constants::ReservedLiteral] = Constants::EndOfModel;
    return true;
}

void deallocate(model& m) {
    Blablabla::log("Deallocating model.");
    m.prev -= Parameters::noVariables;
    m.next -= Parameters::noVariables;
    m.satisfied -= Parameters::noVariables;
    m.reason -= Parameters::noVariables;
    free(m.prev);
    free(m.next);
    free(m.satisfied);
    free(m.reason);
}

void assumeLiteral(model& m, int literal) {
    m.prev[literal] = m.last;
    m.next[literal] = Constants::EndOfModel;
    m.satisfied[literal] = true;
    m.reason[literal] = Constants::AssumedLiteral;
    m.last = literal;
    if(m.head == Constants::EndOfModel) {
        m.head = literal;
    }
    Blablabla::log("Assuming literal " + Blablabla::litToString(literal));
    Blablabla::logModel(m);
}

void propagateLiteral(model& m, int literal, long reason, int* pointer) {
    m.prev[literal] = m.last;
    m.next[literal] = Constants::EndOfModel;
    m.satisfied[literal] = true;
    m.reason[literal] = reason;
    m.last = literal;
    if(m.head == Constants::EndOfModel) {
        m.head = literal;
    }
    Database::setFlag(pointer, Constants::PseudounitBit, Constants::ReasonFlag);
    Blablabla::log("Propagating literal " + Blablabla::litToString(literal));
    Blablabla::logModel(m);
}

void assignSoftConflict(model& m, long reason) {
    m.prev[Constants::ConflictWatchlist] = m.last;
    m.next[Constants::ConflictWatchlist] = Constants::EndOfModel;
    m.satisfied[Constants::ConflictWatchlist] = true;
    m.reasons[Constants::ConflictWatchlist] = reason;
    m.last = Constants::ConflictWatchlist;
}

void deassignLiteral(model& m, database& d, int literal) {
    m.satisfied[literal] = false;
    if(m.reasons[literal] != Constants::AssumedLiteral) {
        Database::setFlag(Database::getPointer(d, m.reasons[literal]), Constants::PseudounitBit, Constants::RedundantFlag);
        m.reasons[literal] = Constants::AssumedLiteral;
    }
}

void dropLiteral(model& m, database& d, int literal) {
    m.next[m.prev[literal]] = m.next[literal];
    m.prev[m.next[literal]] = m.prev[literal];
    deassignLiteral(m, d, literal);
}

bool hardPropagate(model& m, watchlist& wl, database& d) {
    while(m.head != Constants::EndOfModel) {
        Blablabla::log("Processing literal " + Blablabla::litToString(m.head));
        Blablabla::logModel(m);
        if(!WatchList::processList(wl, m, d, -(m.head), Constants::HardPropagation, dummyrup)) { return false; }
        m.head = m.next[m.head];
    }
    m.forced = m.last;
    Blablabla::logModel(m);
    return true;
}

bool softPropagate(model& m, watchlist& wl, database& d, bool& rup) {
    rup = false;
    while(!rup && m.head != Constants::EndOfModel) {
        Blablabla::log("Processing literal " + Blablabla::litToString(m.head));
        if(!WatchList::processList(wl, m, d, -(m.head), Constants::SoftPropagation, rup)) { return false; }
        m.head = m.next[m.head];
    }
    return true;
}

void reset(model& m, database& d) {
    Blablabla::log("Resetting model to stability point");
    for(it = m.next[m.forced]; it != Constants::EndOfModel; it = m.next[it]) {
        deassignLiteral(m, d, it);
    }
    m.next[m.forced] = Constants::EndOfModel;
    m.head = Constants::EndOfModel;
    m.last = m.forced;
    Blablabla::logModel(m);
}

bool isSatisfied(model& m, int literal) {
    return m.satisfied[literal];
}

bool isFalsified(model& m, int literal) {
    return m.satisfied[-literal];
}

bool isUnassigned(model& m, int literal) {
    return !m.satisfied[literal] && !m.satisfied[-literal];
}

bool findNextNonFalsified(model& m, int*& current, int& lit) {
    while((lit = *current) != Constants::EndOfClause) {
        if(!isFalsified(m, lit)) {
            return false;
        }
        ++current;
    }
    return true;
}

}
