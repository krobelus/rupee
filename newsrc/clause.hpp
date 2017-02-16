#ifndef __CLAUSE_H_INCLUDED__
#define __CLAUSE_H_INCLUDED__

#include <stdlib.h>
#include <string>

#include "structs.hpp"

namespace Clause {
    extern int it;

    bool allocateBuffer(clause& c);
    bool reallocateBuffer(clause& c);
    void deallocateBuffer(clause& c);
    bool allocateLiterals(clause& c, int noVars);
    void deallocateLiterals(clause &c);
    bool addLiteral(clause& c, int literal);
    bool addFullLiteral(clause& c, int literal);
    bool closeBuffer(clause& c);
    void resetBuffer(clause& c);
    void resetClause(clause& c);
    void sortClause(clause& c);
	bool equals(clause& c, int* ptr);
    int compare(const void *a, const void *b);
	std::string printClause(clause& c);
}

#endif
