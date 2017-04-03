#include <stdlib.h>
#include <string>

#include "extra.hpp"
#include "structs.hpp"
#include "clause.hpp"

namespace Clause {
    int it;
    int *pit;

    bool allocate(clause& c) {
        Blablabla::log("Allocating clause buffer.");
    	c.clausemax = Parameters::bufferSize;
    	c.clauseused = 0;
    	c.clausearray = (int*) malloc(c.clausemax * sizeof(int));
        c.allocatedlits = 2 * Parameters::noVariables;
        c.clauselits = (bool*) malloc((2 * c.allocatedlits + 1) * sizeof(bool));
    	if(c.clausearray == NULL || c.clauselits == NULL) {
    		Blablabla::log("Error at clause buffer allocation.");
    		Blablabla::comment("Memory management error.");
    		return false;
    	} else {
            c.clauselits += c.allocatedlits;
            for(it = -c.allocatedlits; it <= c.allocatedlits; ++it) {
                c.clauselits[it] = false;
            }
    		return true;
    	}
    }

    bool reallocateBuffer(clause& c) {
        Blablabla::log("Reallocating clause buffer.");
    	c.clausemax *= 2;
    	c.clausearray = (int*) realloc (c.clausearray, c.clausemax * sizeof(int));
    	if(c.clausearray == NULL) {
    		Blablabla::log("Error at clause buffer reallocation.");
    		Blablabla::comment("Memory management error.");
    		return false;
    	} else {
    		return true;
    	}
    }

    bool reallocateFlags(clause& c) {
        Blablabla::log("Reallocating clause buffer.");
        c.clauselits -= c.allocatedlits;
        free(c.clauselits);
        c.allocatedlits *= 2;
        c.clauselits = (bool*) malloc((2 * c.allocatedlits + 1) * sizeof(bool));
        if(c.clauselits == NULL) {
    		Blablabla::log("Error at clause buffer allocation.");
    		Blablabla::comment("Memory management error.");
    		return false;
    	} else {
            c.clauselits += c.allocatedlits;
            for(it = -c.allocatedlits; it <= c.allocatedlits; ++it) {
                c.clauselits[it] = false;
            }
    		return true;
    	}
    }

    void deallocate(clause& c) {
        Blablabla::log("Deallocating clause buffer.");
        c.clauselits -= c.allocatedlits;
        free(c.clauselits);
    	free(c.clausearray);
    }

    bool addLiteral(clause& c, int literal, bool& inbounds, bool& tautology) {
        if(c.clauseused >= c.clausemax) {
    		if(!reallocateBuffer(c)) { return false; }
    	}
        if((inbounds = abs(literal) <= c.allocatedlits) && !(tautology = c.clauselits[-literal])) {
            if(!c.clauselits[literal]) {
                c.clauselits[literal] = true;
                c.clausearray[c.clauseused++] = literal;
            }
        } else {
            c.clausearray[c.clauseused] = Constants::EndOfClause;
            resetBuffer(c);
        }
        return true;
    }

    bool closeBuffer(clause& c) {
        if(c.clauseused >= c.clausemax) {
    		if(!reallocateBuffer(c)) { return false; }
    	}
        c.clausearray[c.clauseused] = Constants::EndOfClause;
        return true;
    }

    void resetBuffer(clause& c) {
        pit = c.clausearray;
        while(*pit != Constants::EndOfClause)  {
            c.clauselits[*(pit++)] = false;
        }
        c.clauseused = 0;
    }

    void sortClause(clause& c) {
        qsort(c.clausearray, c.clauseused, sizeof(int), compare);
    }

    bool equals(clause& c, int* ptr) {
        for(it = 0; it < c.clauseused; ++it) {
            if(ptr[it] != c.clausearray[it]) {       // This check within it < c.clauseused subsumes ptr[it] != 0,
                return false;                       // and so it prevents segmentation fault.
            }
        }
        return (ptr[it] == 0);                      // At this point we know c.clauseused[it] == 0 holds.
    }

    int compare(const void *a, const void *b) {
        return (*(int*)a - *(int*)b);
    }

}
