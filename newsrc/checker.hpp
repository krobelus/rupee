#ifndef __CHECKER_H_INCLUDED__
#define __CHECKER_H_INCLUDED__

namespace Checker {
    bool allocate(checker& c);
    void deallocate(checker& c);
    bool getPremise(checker& c, proof& r, database& d);
    bool getInstruction(checker& c, proof& r, database& d);
    bool initialize(proofreader& i, proof& r, database& d);
    // void firstPremise(proof& r, proofiterator& i);
    // void firstInstruction(proof& r, proofiterator& i);
    // void nextLine(proof& r, proofiterator& i);
    // void prevLine(proof& r, proofiterator& i);
    // void lastInstruction(proof& r, proofiterator& i);
}

#endif
