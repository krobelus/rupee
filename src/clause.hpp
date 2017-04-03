#ifndef __CLAUSE_H_INCLUDED__
#define __CLAUSE_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace Clause {
    extern int it;

    bool allocate(clause& c);
    bool reallocateBuffer(clause& c);
    bool reallocateFlags(clause& c);
    void deallocate(clause& c);
    bool addLiteral(clause& c, int literal, bool& inbounds, bool& tautology);
    bool closeBuffer(clause& c);
    void resetBuffer(clause& c);
    void sortClause(clause& c);
	bool equals(clause& c, int* ptr);
    int compare(const void *a, const void *b);
}

#endif
