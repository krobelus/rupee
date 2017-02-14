#include <stdlib.h>
#include <string>

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
    	c.clausearray = (int*) realloc (c.clausearray, c.clausemax * sizeof(int));
    	if(c.clausearray == NULL) {
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
    	c.clauselits = (bool*) malloc((2 * noVars + 1) * sizeof(bool));
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
    	c.clausearray[c.clauseused++] = literal;
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

    bool closeBuffer(clause& c) {
        if(c.clauseused >= c.clausemax) {
    		if(!reallocateBuffer(c)) { return false; }
    	}
        c.clausearray[c.clauseused] = 0;
        return true;
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

    std::string printClause(clause& c) {
        std::string str;
        if(c.kind == Constants::InstructionDeletion) {
            str = "d:[ ";
        } else {
            str = "i:[ ";
        }
        for(it = 0; it < c.clauseused; ++it) {
            str = str + std::to_string(c.clausearray[it]) + " ";
        }
        return str + "]";
    }

}
