#include <stdlib.h>

#include "structs.hpp"
#include "chain.hpp"

namespace Chain {

int it;
int lit;
int* clause;
int* chit;
long offset;
int literal;

bool allocate(chain& ch) {
    Blablabla::log("Allocating resolution chain.");
    ch.array = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    ch.lits = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    ch.used = ch.array;
    if(ch.array == NULL || ch.lits == NULL) {
        Blablabla::log("Error at resolution chain allocation.");
        Blablabla::comment("Memory management error.");
        return false;
    } else {
        ch.lits += Parameters::noVariables;
        for(it = -Parameters::noVariables; it <= Parameters::noVariables; ++it) {
            ch.lits[it] = false;
        }
    }
    return true;
}

void deallocate(chain& ch) {
    Blablabla::log("Deallocating resolution chain.");
    ch.lits -= Parameters::noVariables;
    free(ch.lits);
    free(ch.array);
}

void initialize(chain& ch, long conflict) {
    ch.conflict = conflict;
    ch.lits[Constants::ConflictWatchlist] = true;
    ch.array[ch.used++] = Constants::ConflictWatchlist;
}

void addClause(chain& ch, int* ptr, int trigger) {
    while((lit = -*(ptr++)) != Constants::EndOfClause) {
        if(!ch.lits[lit] && lit != -trigger) {
            ch.lits[lit] = true;
            ch.array[ch.used++] = lit;
        }
    }
}

bool getReason(chain& ch, model& m, int trigger, long& reason) {
    if(trigger == Constants::ConflictWatchlist) {
        reason = ch.conflict;
        return true;
    } else {
        return ((reason = m.reasons[trigger]) == Constants::AssumedLiteral);
    }
}

void getChain(chain& ch, model& m, database& d, long conflict) {
    initialize(ch, conflict);
    chit = ch.array;
    while(chit < ch.used) {
        pivot = *(++chit);
        if(getReason(ch, m, literal, offset)) {
            addClause(ch, Database::getPointer(d, offset), literal);
        }
    }
    // SORT IT
}

void markChain(chain& ch, database& d) {
    chit = ch.array;
    while(chit < ch.used) {
        getReason(ch, m, *(chit++), offset);
        Database::setFlag(Database::getPointer(d, offset), Constants::VerificationBit, Constants::ScheduledFlag);
    }

}


}
