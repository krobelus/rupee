#include <iostream>

#include "database.hpp"
#include "assignment.hpp"
#include "watchlist.hpp"
#include "extra.hpp"
#include "chain.hpp"

namespace Chain {

long* reasons;
long* chain;
long* chainUsed;
bool* shifts;
int shiftSize;
int* shiftQueue;
int* shiftUsed;
int* discardedQueue;
int* discardedUsed;
int* stackIterator_;
int* clause_;
int pivot_;
int literal_;
long offset_;
long* chainIterator_;
int* ptrA_;
int* ptrB_;

void initialize() {
	std::cout << "Allocating conflict graph." << std::endl;
	reasons = (long*) malloc((2 * Database::noVars + 1) * sizeof(long));
	reasons += Database::noVars;
	shifts = (bool*) malloc((2 * Database::noVars + 1) * sizeof(bool));
	shifts += Database::noVars;
	chain = (long*) malloc((Database::noVars + 2) * sizeof(long));
	chainUsed = chain;
	shiftQueue = (int*) malloc((Database::noVars + 2) * sizeof(int));
	shiftUsed = shiftQueue;
	discardedQueue = (int*) malloc((Database::noVars + 2) * sizeof(int));
	discardedUsed = discardedQueue;
	for(literal_ = -Database::noVars; literal_ <= Database::noVars; ++literal_) {
		reasons[literal_] = 0;
		shifts[literal_] = false;
	}
}

void deallocate() {
	std::cout << "Deallocating conflict graph." << std::endl;
	reasons -= Database::noVars;
	free(reasons);
	shifts -= Database::noVars;
	free(shifts);
	free(chain);
	free(shiftQueue);
	free(discardedQueue);
}

int* findReason(long offset) {
	stackIterator_ = Assignment::stack;
	while(stackIterator_ < Assignment::stackUsed) {
		if(reasons[*stackIterator_] == offset) {
			return stackIterator_;
		} else {
			++stackIterator_;
		}
	}
	return 0;
}

void addResolution(int* &iterator) {
	pivot_ = -*iterator;							//I am storing the complement of the pivot for efficiency reasons. Probably overoptimizing...
	if((offset_ = reasons[-pivot_]) != 0) {			//If the pivot is not an assumption,
		clause_ = Database::getPointer(offset_);	//then retrieve the reason clause.
		while((literal_ = -*(clause_++)) != 0) {		//For every literal in that clause other than the pivot,
			if(literal_ != pivot_ && !shifts[literal_]) {	//its negation is a precedent of the pivot. If I have not seen it yet,
				shifts[literal_] = true;					//then I add it to the shift
				*(shiftUsed++) = literal_;					//and remember that I have seen it.
				++shiftSize;
			}
		}
		++iterator;									//In that case, go for the next literal in the shift.
	} else {										//If the pivot is an assumption,
		*(discardedUsed++) = -pivot_;				//then I remove it from the chain.
		*iterator = *(--shiftUsed);					//I still need to store it in the discarded queue so that I can clean the shifts quickly.
		--shiftSize;
	}
}

void cleanShift() {
	stackIterator_ = shiftUsed;
	while((--stackIterator_) >= shiftQueue) {
		shifts[*stackIterator_] = false;
	}
	stackIterator_ = discardedUsed;
	while((--stackIterator_) >= discardedQueue) {
		shifts[*stackIterator_] = false;
	}
}

int compareShifts(const void* a, const void* b){
	ptrA_ = Assignment::stackPointers[*(int*) a];
	ptrB_ = Assignment::stackPointers[*(int*) b];
	if(ptrA_ < ptrB_) {
		return 1;
	} else if (ptrA_ > ptrB_) {
		return -1;
	} else {
		return 0;
	}
}

void generateChain() {
	Tools::comment("Generating resolution chain.");
	for(int i = -Database::noVars; i <= Database::noVars; ++i) {
		if(shifts[i]) {
			Tools::comment("still " + std::to_string(i));
		}
	}
	shiftUsed = shiftQueue;
	discardedUsed = discardedQueue;
	chainUsed = chain;
	if(reasons[0] == 0) {
		reasons[0] = *(WatchList::conflictList);
	}
	Assignment::stackPointers[0] = Assignment::stackUsed;	//This is a hack to avoid a very annoying check when reordering the resolution chain.
	*(shiftUsed++) = 0;
	shiftSize = 1;
	stackIterator_ = shiftQueue;
	while(stackIterator_ < shiftUsed) {
		addResolution(stackIterator_);
	}
	qsort(shiftQueue, shiftSize, sizeof(int), compareShifts);
	stackIterator_ = shiftQueue;
	while(stackIterator_ < shiftUsed) {
		*(chainUsed++) = reasons[*stackIterator_++];
	}
	cleanShift();					//Clean the shift switches.
	Tools::resolutionChain();
}

void markChain() {
	chainIterator_ = chain;
	while(chainIterator_ < chainUsed) {
		Database::setFlag(*(chainIterator_++), Constants::VerificationFlagDatabase, Constants::KindVerify);
	}
}

}
