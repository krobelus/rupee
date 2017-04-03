#include <stdlib.h>

#include "structs.hpp"
#include "database.hpp"
#include "watchlist.hpp"
#include "extra.hpp"
#include "latency.hpp"

namespace Latency {

int it;
int lit;
long* watch;
long offset;
int* clause;

bool allocate(latency& x) {
    Blablabla::log("Allocating latency structures.");
    x.array = (long*) malloc(Parameters::hashDepth * sizeof(long));
    x.max = Parameters::hashDepth;
    x.used = Constants::EndOfWatchList;
    x.resolvent = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    x.original = x.added = x.resolvent;
    x.lits = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    if(x.array == NULL || x.resolvent == NULL || x.lits == NULL) {
        Blablabla::log("Error at latency structures allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    } else {
        x.lits += Parameters::noVariables;
        for(it = -Parameters::noVariables; it <= Parameters::noVariables; ++it) {
            x.lits[it] = false;
        }
        *(x.array) = 0;
        return true;
    }
}

bool reallocate(latency& x) {
    Blablabla::log("Reallocating latency structures.");
    x.max = x.max * 4;
    x.array = (long*) realloc(x.array, x.max * sizeof(long));
    if(x.array == NULL) {
        Blablabla::log("Error at latency structures allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    } else {
        return true;
    }
}

void deallocate(latency& x) {
    Blablabla::log("Deallocating latency structures.");
    x.lits -= Parameters::noVariables;
    free(x.array);
    free(x.resolvent);
    free(x.lits);
}

void addLiteral(latency& x, int literal) {
    *(x.added++) = literal;
    x.lits[literal] = true;
}

bool addClause(latency& x, long offset) {
    if(x.used >= x.max) {
        if(!reallocate(x)) { return false; }
    }
    x.array[x.used++] = offset;
    x.array[x.used] = Constants::EndOfWatchList;
    return true;
}

bool findResolvableClauses(latency &x, watchlist &wl, database& d) {
    for(it = -Parameters::noVariables; it <= Parameters::noVariables; ++it) {
        watch = wl.array[it];
        while((offset = *(watch++)) != Constants::EndOfWatchList) {
            clause = Database::getPointer(d, offset);
            if((it == Constants::ConflictWatchlist || *clause == it)
                    && Database::containsLiteral(d, clause, x.pivot)) {
                if(!addClause(x, offset)) { return false; }
            }
        }
    }
    return true;
}

void saveOriginal(latency& x) {
    x.original = x.added;
}

void restore(latency& x) {
    while(x.added >= x.original) {
        x.lits[*(x.added--)] = false;
    }
    x.added = x.original;
}

void unload(latency& x) {
    while(x.added >= x.resolvent) {
        x.lits[*(x.added--)] = false;
    }
    x.added = x.original = x.resolvent;
}

void loadClause(latency& x, int* ptr, int pivot) {
    x.pivot = pivot;
    while((lit = *(ptr++)) != Constants::EndOfClause) {
        if(lit != pivot) {
            addLiteral(x, lit);
        }
    }
    saveOriginal(x);
}

bool computeResolvent(latency& x, int* ptr) {
    std::string clause;
    while((lit = *(ptr++)) != Constants::EndOfClause) {
        if(lit != -x.pivot && !x.lits[lit]) {
            if(x.lits[-lit]) {
                return false;
            } else {
                addLiteral(x, lit);
            }
        }
    }
    return true;
}


}
