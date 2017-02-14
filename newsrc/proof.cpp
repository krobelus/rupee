#include <iostream>
#include <string>

#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"

namespace Proof {

bool allocate(proof& r) {
	Blablabla::log("Allocating proof list.");
	r.proofmax = Parameters::databaseSize;
	r.proofused = 0;
	r.proofarray = (long*) malloc (r.proofmax * sizeof(long));
	r.proofpivots = (int*) malloc (r.proofmax * sizeof(int));
	if(r.proofarray == NULL || r.proofpivots == NULL) {
		Blablabla::log("Error at proof list allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

bool reallocate(proof& r) {
	Blablabla::log("Reallocating proof list.");
	r.proofmax = (r.proofmax * 3) >> 1;
	r.proofarray = (long*) realloc (r.proofarray, r.proofmax * sizeof(long));
	r.proofpivots = (int*) realloc (r.proofpivots, r.proofmax * sizeof(int));
	if(r.proofarray == NULL || r.proofpivots == NULL) {
		Blablabla::log("Error at proof list reallocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void deallocate(proof& r) {
	Blablabla::log("Deallocating proof list.");
	free(r.proofarray);
	free(r.proofpivots);
}

bool storeInstruction(proof& r, long offset, int pivot, bool kind) {
	if(r.proofused >= r.proofmax) {
		if(!reallocate(r)) { return false; }
	}
	r.proofarray[r.proofused] = offset;
	r.proofpivots[r.proofused++] = (pivot << 1) + (int) kind;
	return true;
}

bool getInstruction(proof& r, int i, long& offset, int& pivot, bool& kind) {
	if(i < 0 || i >= r.proofused) { return false; }
	offset = r.proofarray[i];
	pivot = r.proofpivots[i];
	kind = pivot % 2;
	pivot >>= 1;
	return true;
}

void log(proof& r, database& d) {
	int i;
	long offset;
	int pivot;
	bool kind;
	std::string str;
	Blablabla::log("Proof:");
	Blablabla::increase();
	for(i = 0; i < r.proofused; ++i) {
		getInstruction(r, i, offset, pivot, kind);
		if(kind == Constants::InstructionIntroduction) {
			str = "+++ ";
		} else {
			str = "--- ";
		}
		Blablabla::log(str + Database::offsetToString(d, offset));
	}
	Blablabla::decrease();
}



}
