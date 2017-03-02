#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"

namespace ProofReader {

bool allocate(proofreader& i) {
    if(!WatchList::allocate(i.watches)) { return false; }
    if(!Trail::allocate(i.trail)) { return false; }
    return true;
}

void deallocate(proofreader& i) {
    WatchList::deallocate(i.watches);
    Trail::deallocate(i.trail);
}

void getPremise(proofreader& i, proof& r, database& d) {
	i.offset = r.proofarray[i.position];
	i.pointer = Database::getPointer(d, i.offset);
}

void getInference(proofreader& i, proof& r, database& d) {
	i.offset = r.proofarray[i.position];
	i.pointer = Database::getPointer(d, i.offset);
	i.pivot = r.proofpivots[i.position];
	i.kind = i.pivot % 2;
	i.pivot >>= 1;
}

bool initialize(proofreader& i, proof& r, database& d) {
    for(i.position = 0; i.position < r.noPremises; ++i.position) {
        getPremise(i, r, d);
        Database::setClauseActive(d, i.pointer);
        if(!WatchList::insertClause(i.watches, i.offset, i.pointer)) { return false; }
    }
    return true;
}

bool advanceForward(proofreader& i, proof& r, database& d) {
    if(i.position >= r.proofused) {
        return false;
    } else {
        getInference(i, r, d);
        return true;
    }
}
