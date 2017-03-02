#ifndef __PROOFREADER_H_INCLUDED__
#define __PROOFREADER_H_INCLUDED__

namespace ProofReader {
    bool allocate(proofreader& i);
    void deallocate(proofreader& i);
    bool getPremise(proof& r, database& d, proofiterator& i);
    bool getInstruction(proof& r, database& d, proofiterator& i);
    bool initialize(proofreader& i, proof& r, database& d);
    // void firstPremise(proof& r, proofiterator& i);
    // void firstInstruction(proof& r, proofiterator& i);
    // void nextLine(proof& r, proofiterator& i);
    // void prevLine(proof& r, proofiterator& i);
    // void lastInstruction(proof& r, proofiterator& i);
}

#endif
