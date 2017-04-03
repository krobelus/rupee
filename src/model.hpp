#ifndef __MODEL_H_INCLUDED__
#define __MODEL_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace Model {
    bool allocate(model& m);
    void deallocate(model& m);

    void assumeLiteral(model& m, int literal);
    void propagateLiteral(model& m, int literal, long reason, int* pointer);
    void assignSoftConflict(model& m, long reason);
    void deassignLiteral(model& m, database& d, int literal);

    bool hardPropagate(model& m, watchlist& wl, database& d);
    bool softPropagate(model& m, watchlist& wl, database& d, bool& rup);
    void reset(model& m, database& d);

    bool isSatisfied(model& m, int literal);
    bool isFalsified(model& m, int literal);
    bool isUnassigned(model& m, int literal);
    bool findNextNonFalsified(model& m, int*& current, int& lit);
}

#endif
