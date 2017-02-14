#ifndef __PROOF_H_INCLUDED__
#define __PROOF_H_INCLUDED__

#include <stdlib.h>

#include "database.hpp"

struct proof {
    long* proofarray;
    int* proofpivots;
    int proofmax;
    int proofused;
};

namespace Proof {
    bool allocate(proof& r);
    bool reallocate(proof& r);
    void deallocate(proof& r);
    bool storeInstruction(proof& r, long offset, int pivot, bool kind);
    bool getInstruction(proof& r, int i, long& offset, int& pivot, bool& kind);
    void log(proof& r, database& d);
}

#endif
