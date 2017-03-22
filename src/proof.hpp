#ifndef __PROOF_H_INCLUDED__
#define __PROOF_H_INCLUDED__

#include "structs.hpp"

namespace Proof {
    bool allocate(proof& r);
    bool reallocate(proof& r);
    void deallocate(proof& r);
    bool storePremise(proof& r, long offset);
    bool storeInstruction(proof& r, long offset, int pivot, bool kind);
}

#endif
