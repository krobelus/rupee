#include <stdlib.h>

#include "extra.hpp"
#include "clause.hpp"

namespace Clause {
    int it;

    bool allocateBuffer(clause& c) {
        Blablabla::log("Allocating clause buffer.");
    	c.clausemax = Parameters::bufferSize;
    	c.clauseused = 0;
    	c.clausearray = (int*) malloc(c.clausemax * sizeof(int));
    	if(c.clausearray == NULL) {
    		Blablabla::log("Error at clause buffer allocation.");
    		Blablabla::comment("Memory management error.");
    		return false;
    	} else {
    		return true;
    	}
    }

    bool reallocateBuffer(clause& c) {
        Blablabla::log("Reallocating clause buffer.");
    	c.clausemax *= 2;
    	(int*) realloc (c.clausearray, c.clausemax * sizeof(int));
    	if(b.clausearray == NULL) {
    		Blablabla::log("Error at clause buffer reallocation.");
    		Blablabla::comment("Memory management error.");
    		return false;
    	} else {
    		return true;
    	}
    }

    void deallocateBuffer(clause& c) {
        Blablabla::log("Deallocating clause buffer.");
    	free(c.clausearray);
    }

    bool allocateLiterals(clause& c, int noVars) {
        Blablabla::log("Allocating literal buffer.");
    	c.clauselits = (int*) malloc((2 * noVars + 1) * sizeof(int));
    	if(c.clausearray == NULL) {
    		Blablabla::log("Error at literal buffer allocation.");
    		Blablabla::comment("Memory management error.");
    		return false;
    	} else {
            c.clauselits += noVars;
            for(it = 0; it <= noVars; ++it) {
                c.clauselits[it] = c.clauselits[-it] = false;
            }
    		return true;
    	}
    }

    void deallocateLiterals(clause &c) {
        Blablabla::log("Deallocating literal buffer.");
    	free(c.clauselits);
    }

    bool addLiteral(clause& c, int literal) {
        if(c.clauseused >= c.clausemax) {
    		if(!reallocateBuffer(c)) { return false; }
    	}
    	b.clausearray[b.clauseused++] = literal;
        return true;
    }

    bool addFullLiteral(clause& c, int literal) {
        if(c.clauselits[-literal]) {
            return false;
        } else {
            c.clausearray[c.clauseused++] = literal;
            c.clauselits[literal] = true;
            return true;
        }
    }

    void closeBuffer(clause& c) {
        if(c.clauseused >= c.clausemax) {
    		if(!reallocateBuffer(c)) { return false; }
    	}
        c.clausearray[c.clauseused] = 0;
    }

    void resetBuffer(clause& c) {
        c.clauseused = 0;
    }

    void resetClause(clause& c) {
        for(it = 0; it < c.clauseused; ++it) {
            c.clauselits[c.clausearray[it]] = false;
        }
        c.clauseused = 0;
    }

    void sortClause(clause& c) {
        qsort(b.bufferarray, b.bufferused, sizeof(int), compare)
    }

    int compare(const void *a, const void *b) {
        return (*(int*)a - *(int*)b);
    }

}
