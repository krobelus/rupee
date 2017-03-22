#ifndef __CHECKER_H_INCLUDED__
#define __CHECKER_H_INCLUDED__

#include "structs.hpp"

namespace Checker {

bool allocate(checker& c);
void deallocate(checker& c);

void getPremise(checker& c, proof& r, database& d);
void getInstruction(checker& c, proof& r, database& d);

bool initialize(checker& c, proof& r, database& d);
bool preprocessProof(checker& c, proof& r, database& d, bool& conflict);
bool verifyProof(checker& c, proof& r, database& d, bool& verified);

bool preprocessInstruction(checker& c, proof& r, database& d, bool& stop);
bool verifyInstruction(checker& c, proof& r, database& d, bool& verified);

bool introduceClause(checker &c, database& d);
bool deleteClause(checker &c, database& d);

bool checkRup(checker &c, database& d, int* clause, bool& rup);
bool checkRat(checker &c, database& d, int* clause, int pivot, bool& rat);

}

#endif
