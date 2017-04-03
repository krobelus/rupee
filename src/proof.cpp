#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"
#include "proof.hpp"

namespace Proof {

bool allocate(proof& r) {
	Blablabla::log("Allocating proof list.");
	r.max = Parameters::databaseSize;
	r.used = 0;
	r.array = (long*) malloc (r.max * sizeof(long));
	r.pivots = (int*) malloc (r.max * sizeof(int));
	r.kinds = (bool*) malloc(r.max * sizeof(bool));
	r.noPremises = 0;
	if(r.array == NULL || r.pivots == NULL || r.kinds == NULL) {
		Blablabla::log("Error at proof list allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

bool reallocate(proof& r) {
	Blablabla::log("Reallocating proof list.");
	r.max *= 4;
	r.array = (long*) realloc (r.array, r.max * sizeof(long));
	r.pivots = (int*) realloc (r.pivots, r.max * sizeof(int));
	r.kinds = (bool*) realloc (r.kinds, r.max * sizeof(bool));
	if(r.array == NULL || r.pivots == NULL || r.kinds == NULL) {
		Blablabla::log("Error at proof list reallocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void deallocate(proof& r) {
	Blablabla::log("Deallocating proof list.");
	free(r.array);
	free(r.pivots);
}

bool storePremise(proof& r, long offset) {
	++(r.noPremises);
	return storeInstruction(r, offset, 0, Constants::InstructionIntroduction);
}

bool storeInstruction(proof& r, long offset, int pivot, bool kind) {
	if(r.used >= r.max) {
		if(!reallocate(r)) { return false; }
	}
	r.array[r.used] = offset;
	r.pivots[r.used] = pivot;
	r.kinds[r.used++] = kind;
	return true;
}

}
