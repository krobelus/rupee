#include <iostream>
#include <string>

#include "structs.hpp"
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
	r.noPremises = 0;
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

bool storePremise(proof& r, long offset) {
	++(r.noPremises);
	return storeInstruction(r, offset, 0, false);
}

bool storeInstruction(proof& r, long offset, int pivot, bool kind) {
	if(r.proofused >= r.proofmax) {
		if(!reallocate(r)) { return false; }
	}
	r.proofarray[r.proofused] = offset;
	r.proofpivots[r.proofused++] = (pivot << 1) + (int) kind;
	return true;
}

// void firstPremise(proof& r, proofiterator& i) {
// 	i.position = 0;
// }
//
// void firstInstruction(proof& r, proofiterator& i) {
// 	i.position = r.noPremises;
// }
//
// void nextLine(proof& r, proofiterator& i) {
// 	++(i.position);
// }
//
// void prevLine(proof& r, proofiterator& i) {
// 	--(i.position);
// }
//
// void lastInstruction(proof& r, proofiterator& i) {
// 	i.position = r.proofused - 1;
// }
//
// bool getPremise(proof& r, database& d, proofiterator& i) {
// 	if(i.position < 0 || i.position >= r.noPremises) { return false; }
// 	i.offset = r.proofarray[i.position];
// 	i.pointer = Database::getPointer(d, i.offset);
// 	return true;
// }
//
// bool getInstruction(proof& r, database& d, proofiterator& i) {
// 	if(i.position < r.noPremises || i.position >= r.proofused) { return false; }
// 	i.offset = r.proofarray[i.position];
// 	i.pointer = Database::getPointer(d, i.offset);
// 	i.pivot = r.proofpivots[i.position];
// 	i.kind = i.pivot % 2;
// 	i.pivot >>= 1;
// 	return true;
// }

void log(proof& r, database& d) {
	proofiterator i;
	std::string str;
	Blablabla::log("Proof:");
	Blablabla::increase();
	for(firstInstruction(r,i); getInstruction(r,d,i); nextLine(r,i)) {
		if(i.kind == Constants::InstructionIntroduction) {
			str = "+++ ";
		} else {
			str = "--- ";
		}
		Blablabla::log(str + Database::offsetToString(d, i.offset));
	}
	Blablabla::decrease();
}



}
