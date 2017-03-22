#ifndef __REVISION_H_INCLUDED__
#define __REVISION_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace Revision {
    bool allocate(revision& v);
    void deallocate(revision& v);

    void addLiteral(int lit);
    bool isInCone(revision& v, long offset, int lit, database& d);
    void getCone(revision& v, model& m, database& d, int* ptr);
    bool reviseList(revision& v, watchlist& wl, model& m, database& d);
}

#endif
