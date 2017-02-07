#include <iostream>

#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"

namespace Proof {

long* proof;
int max;
int used;

void allocate() {
	Blablabla::log("Allocating proof list.");
	max = Parameters::databaseSize;
	proof = (long*) malloc (max * sizeof (long ));
	used = 0;
	if(proof == NULL) {
		Blablabla::log("Error at proof list allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void reallocate() {
	Tools::comment("Reallocating proof list.");
	Proof::max = Tools::leap(Proof::max);
	proof = (long*) realloc (proof, Proof::max * sizeof(long));
}

void deallocate() {
	std::cout << "Deallocating proof list." << std::endl;
	free(proof);
}

void storeInstruction(long offset, int kind) {
	if(used == Proof::max) {
		Proof::reallocate();
	}
	proof[used++] = (offset << Constants::ExtraBitsProof) + kind;
}

void getInstruction(long* instruction, long &offset, int &kind) {
	offset = *instruction;
	kind = offset % 2;
	offset >>= 1;
}

void print() {
	std::cout << "PROOF:" << std::endl;
	for(int i = 0; i < used; ++i) {
		if(proof[i] % 2 == Constants::KindDeletion) { std::cout << "--- "; } else { std::cout << "+++ "; }
		Database::printClause(proof[i] >> Constants::ExtraBitsProof);
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

}
