#include <iostream>

#include "extra.hpp"
#include "watchlist.hpp"
#include "database.hpp"
#include "resolvent.hpp"

namespace Resolvent {

long* candidatesQueue;
int candidatesUsed;
int candidatesMax;
int* resolventClause;
int* resolventOriginal;
int* resolventUsed;
bool* resolventLiterals;
int pivot;
long offset_;
int literalIterator_;
long* watchIterator_;
int* clause_;
int readLiteral_;
bool done_;
int* resolventIterator_;

void initialize() {
	Tools::comment("Allocating resolution system.");
	candidatesMax = Constants::SmallAllocation;
	candidatesUsed = 0;
	candidatesQueue = (long*) malloc(candidatesMax * sizeof(long));
	resolventClause = (int*) malloc((Database::noVars + 2) * sizeof(int));
	resolventOriginal = resolventUsed = resolventClause;
	resolventLiterals = (bool*) malloc((Database::noVars + 1) * sizeof(bool));
	resolventLiterals += Database::noVars;
	for(literalIterator_ = -Database::noVars; literalIterator_ <= Database::noVars; ++literalIterator_) {
		resolventLiterals[literalIterator_] = false;
	}
}

void reallocate() {
	Tools::comment("Reallocating resolution system.");
	candidatesMax *= 2;
	candidatesQueue = (long*) realloc(candidatesQueue, candidatesMax * sizeof(long));
}

void deallocate() {
	Tools::comment("Deallocating resolution system.");
	free(candidatesQueue);
	free(resolventClause);
	resolventLiterals -= Database::noVars;
	free(resolventLiterals);
}

bool containsLiteral(int* &clause, int literal) {
	while((readLiteral_ = *(clause++)) != 0) {
		if(readLiteral_ == literal) {
			return true;
		}
	}
	return false;
}

void addCandidate(long offset) {
	if(candidatesUsed == candidatesMax) {
		reallocate();
	}
	candidatesQueue[candidatesUsed++] = offset;
}

void findResolutionCandidates(int literal) {
	literal *= -1;
	candidatesUsed = 0;
	if((offset_ = *(WatchList::unitList[literal])) != 0) {
		addCandidate(offset_);
	}
	for(literalIterator_ = -Database::noVars; literalIterator_ <= Database::noVars; ++literalIterator_) {
		if(literalIterator_ != 0) {
			watchIterator_ = WatchList::watchList[literalIterator_];
			while((offset_ = *(watchIterator_++)) != 0) {
				clause_ = Database::getPointer(offset_);
				if(*clause_ == literalIterator_ && containsLiteral(clause_, literal)) {
					addCandidate(offset_);
				}
			}
		}
	}
	addCandidate(0);
}

void setupResolutionSystem(int* clause, int literal) {
	pivot = literal;
	resolventUsed = resolventClause;
	while((readLiteral_ = *(clause++)) != 0) {
		if(readLiteral_ != pivot) {
			resolventLiterals[readLiteral_] = true;
			*(resolventUsed++) = readLiteral_;
		}
	}
	resolventOriginal = resolventUsed;
}

bool computeNextResolvent(long* &candidateIterator) {
	while(true) {
		offset_ = *candidateIterator;
		if(offset_ == 0) {
			return false;
		} else {
			clause_ = Database::getPointer(offset_);
			resolventUsed = resolventOriginal;
			done_ = false;
			while(!done_ && (readLiteral_ = -*(clause_++)) != 0) {
				if(resolventLiterals[readLiteral_]) {
					done_ = true;
				} else if(readLiteral_ != pivot) {
					*(resolventUsed++) = readLiteral_;
				}
			}
			if(done_) {
				++candidateIterator;
			} else {
				return true;
			}
		}
	}
}

void cleanResolvent() {
	resolventIterator_ = resolventClause;
	while(resolventIterator_ < resolventOriginal) {
		resolventLiterals[*(resolventIterator_++)] = false;
	}
}

}
