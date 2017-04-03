#include <stdlib.h>

#include "structs.hpp"
#include "model.hpp"
#include "database.hpp"
#include "extra.hpp"
#include "chain.hpp"

namespace Chain {

int it;
int lit;
int* clause;
long offset;
int literal;
model* mm;
int* c1;
int* c2;

bool allocate(chain& ch) {
    Blablabla::log("Allocating resolution chain.");
    ch.array = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    ch.lits = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    ch.used = 0;
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

void addClause(chain& ch, int* ptr, int trigger) {
    while((lit = -*(ptr++)) != Constants::EndOfClause) {
        if(!ch.lits[lit] && lit != -trigger) {
            ch.lits[lit] = true;
            ch.array[ch.used++] = lit;
        }
    }
}

bool getReason(chain& ch, model& m, int trigger, long& reason) {
    return ((reason = m.reasons[trigger]) != Constants::AssumedLiteral);
}

void getChain(chain& ch, model& m, database& d) {
    ch.lits[Constants::ConflictWatchlist] = true;
    ch.array[ch.used++] = Constants::ConflictWatchlist;
    it = 0;
    while(it < ch.used) {
        literal = ch.array[it];
        if(getReason(ch, m, literal, offset)) {
            addClause(ch, Database::getPointer(d, offset), literal);
            ++it;
        } else {
            ch.lits[literal] = false;
            ch.array[it] = ch.array[--ch.used];
        }
    }
    mm = &m;
    qsort(ch.array, ch.used, sizeof(int), compare);
}

int compare(const void *a, const void *b) {
    c1 = mm->positions[*(int*) a];
    c2 = mm->positions[*(int*) b];
    return (int)(c1 < c2) - (int)(c2 < c1);
}

void markChain(chain& ch, model& m, database& d) {
    it = 0;
    while(it < ch.used) {
        if(getReason(ch, m, ch.array[it], offset)) {
            Database::setFlag(Database::getPointer(d, offset), Constants::VerificationBit, Constants::ScheduledFlag);
        }
        ch.lits[ch.array[it++]] = false;
    }
    ch.used = 0;
}


}
