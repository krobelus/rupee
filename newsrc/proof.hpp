#ifndef __PROOF_H_INCLUDED__
#define __PROOF_H_INCLUDED__

#include "database.hpp"

namespace Proof {
    bool allocate(proof& r);
    bool reallocate(proof& r);
    void deallocate(proof& r);
    bool storePremise(proof& r, long offset);
    bool storeInstruction(proof& r, long offset, int pivot, bool kind);
    bool getPremise(proof& r, database& d, proofiterator& i);
    bool getInstruction(proof& r, database& d, proofiterator& i);
    void firstPremise(proof& r, proofiterator& i);
    void firstInstruction(proof& r, proofiterator& i);
    void nextLine(proof& r, proofiterator& i);
    void prevLine(proof& r, proofiterator& i);
    void lastInstruction(proof& r, proofiterator& i);
    void log(proof& r, database& d);
}

#endif
