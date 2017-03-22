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

bool allocate(model& m) {
    Blablabla::log("Allocating model.");
    m.start = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    m.satisfied = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    m.positions = (int**) malloc((2 * Parameters::noVariables + 1) * sizeof(int*));
    m.reasons = (long*) malloc((2 * Parameters::noVariables + 1) * sizeof(long));
    if(m.start == NULL || m.positions == NULL || m.satisfied == NULL) {
        Blablabla::log("Error at model allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    }
    m.forced = m.head = m.used;
    m.satisfied += Parameters::noVariables;
    m.positions += Parameters::noVariables;
    for(it = 0; it <= Parameters::noVariables; ++it) {
        m.satisfied[it] = m.satisfied[-it] = false;
        m.positions[it] = m.positions[-it] = NULL;
        m.reasons[it] = 0;
    }
    return true;
}

void deallocate(model& m) {
    Blablabla::log("Deallocating model.");
    m.satisfied -= Parameters::noVariables;
    m.positions -= Parameters::noVariables;
    m.reasons -= Parameters::noVariables;
    free(m.start);
    free(m.satisfied);
    free(m.positions);
    free(m.reasons)
}

void assumeLiteral(model& m, int literal) {
    m.satisfied[literal] = true;
    *m.used = literal;
    m.positions[literal] = m.used++;
    m.reasons[literal] = Constants::AssumedLiteral;
    Blablabla::log("Assuming literal " + Blablabla::litToString(literal));
    Blablabla::logModel(c.stack);
}

void propagateLiteral(model& m, int literal, long reason, int* pointer) {
    m.satisfied[literal] = true;
    *m.used = literal;
    m.positions[literal] = m.used++;
    m.reasons[literal] = reason;
    Database::setFlag(pointer, Constants::PseudounitBit, Constants::ReasonFlag);
    Blablabla::log("Propagating literal " + Blablabla::litToString(literal));
    Blablabla::logModel(c.stack);
}

void deassignLiteral(model& m, int literal) {
    m.satisfied[literal] = false;
}

bool hardPropagate(model& m, watchlist& wl, database& d) {
    while(m.head < m.used) {
        Blablabla::log("Processing literal " + Blablabla::litToString(*(m.head)) + ".");
        if(!WatchList::processList(m, wl, d, -*(m.head++), Constants::HardPropagation, dummyrup)) { return false; }
    }
    m.forced = m.head;
    return true;
}

bool softPropagate(model& m, watchlist& wl, database& d, bool& rup) {
    rup = false;
    while(!rup && m.head < m.used) {
        Blablabla::log("Processing literal " + Blablabla::litToString(*(m.head)) + ".");
        if(!WatchList::processList(m, wl, d, -*(m.head++), Constants::SoftPropagation, rup)) { return false; }
    }
    return true;
}

void reset(model& m) {
    Blablabla::log("Resetting model to stability point.");
    while(m.used >= m.forced) {
        deassignLiteral(m, (m.used--)*);
    }
    m.head = m.used = m.forced;
    Blablabla::logModel(m);
}

bool isSatisfied(model& m, int literal) {
    return satisfied[literal];
}

bool isFalsified(model& m, int literal) {
    return satisfied[-literal];
}

bool isUnassigned(model& m, int literal) {
    return !satisfied[literal] && !satisfied[-literal];
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
