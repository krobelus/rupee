#include <iostream>

#include "database.hpp"
#include "watchlist.hpp"
#include "chain.hpp"
#include "reprocess.hpp"
#include "assignment.hpp"
#include "extra.hpp"


namespace Assignment {

int* stack;
int* stackCurrent;
int* stackUsed;
int* savedCurrent;
int* savedUsed;
bool savedConflict;
bool* falsified;
int** stackPointers;
bool conflict;
int watchIterator_;
int literal_;
int* stackLeft_;
int* stackRight_;
bool unpropagate_;

void initialize() {
	Tools::comment("Allocating assignment stack.");
	stack = (int*) malloc((Database::noVars + 2) * sizeof(int));
	stackCurrent = stackUsed = stack;
	falsified = (bool*) malloc((2 * Database::noVars + 1) * sizeof(bool));
	falsified += Database::noVars;
	stackPointers = (int**) malloc((2 * Database::noVars + 1) * sizeof(int*));
	stackPointers += Database::noVars;
	conflict = savedConflict = false;
	for(literal_ = 0; literal_ <= Database::noVars; ++literal_) {
		falsified[literal_] = falsified[-literal_] = false;
	}
}

void deallocate() {
	Tools::comment("Deallocating assignment stack.");
	free(stack);
	falsified -= Database::noVars;
	free(falsified);
	stackPointers -= Database::noVars;
	free(stackPointers);
}

// Assigns a literal by setting the corresponding falsified slot and appending it to the stack.
// The distinction between forced and soft literals is not currently enforced.
// In the future, assigning a forced literal will also add the offset to the reason hashset to improve literal dropping.
// An assumption is essentially the same as a soft assignment; 0 is given as a reason to keep track of what are assumptions (remember, no clause has offset 0).
// WARNING no check is performed to detect if the assigned literal was previously ed; if this is the case, things quickly get unsound.
void assignHardLiteral(int lit, long offset) {
	falsified[-lit] = true;
	Chain::reasons[lit] = offset;
	stackPointers[lit] = stackUsed;
	*(stackUsed++) = lit;
}

void assignSoftLiteral(int lit, long offset) {
	assignHardLiteral(lit, offset);
}

void unassignSoftLiteral(int lit) {
	unassignHardLiteral(lit);
}

void unassignHardLiteral(int lit) {
	falsified[-lit] = false;
}

void assignAssumedLiteral(int lit) {
	assignSoftLiteral(lit, 0);
}


// Propagates the top-level stack. True is returned iff propagation terminates without conflict.
// This works by lazy evaluation of boolean expressions in C++.
// That's relevant because one should NOT continue propagating or doing anything at all after a conflict is dectected.
// If this happens, watching information might be lost forever, and soundness upon modification of the top-level reasons cannot be guaranteed.
// If a problem arises, replace the lazy evaluation command by the commented code.
bool propagateHard() {
	Tools::increaseCommentLevel();
	if(conflict) {
		Tools::comment("Stack is in conflicting state.");
		Tools::decreaseCommentLevel();
		return false;
	} else if(Reprocess::reprocess()) {
		Tools::decreaseCommentLevel();
		return false;
	} else if(WatchList::processUnits()) {
		Tools::decreaseCommentLevel();
		return false;
	} else if(processStackHard()) {
		Tools::decreaseCommentLevel();
		return false;
	} else {
		Tools::decreaseCommentLevel();
		return true;
	}
}

// Same as propagateHard(), but units and reprocessing need not to be performed (they are assumed to take place before this).
// Also, instead of processStackHard(), processStackSoft() is called; but currently they are implemented the same.
bool propagateSoft() {
	if(conflict) {
		Tools::comment("Stack is in conflicting state.");
		return false;
	} else if(processStackSoft()) {
		return false;
	} else {
		return true;
	}
}

// Processes the remaining part of the stack until there is a conflict, in which case true is returned;
// or the stack is exhausted, in which case false is returned.
bool processStackHard() {
	Tools::comment("Stack processing phase.");
	Tools::increaseCommentLevel();
	while(stackCurrent < stackUsed) {
		literal_ = -*stackCurrent;
		Tools::comment("Processing literal " + std::to_string(-literal_) + ". Stack state:   " + Tools::processingStackToString());
		Tools::increaseCommentLevel();
		Tools::comment("Watchlist for literal " + std::to_string(literal_) + ":");
		Tools::watchList(literal_);
		watchIterator_ = 0;
		while(WatchList::processWatchHard(literal_, watchIterator_)) {}
		if(conflict) {
			Tools::decreaseCommentLevel();
			Tools::decreaseCommentLevel();
			Tools::comment("Stack is in conflicting state.");
			force();
			return true;
		} else {
			Tools::decreaseCommentLevel();
			++stackCurrent;
		}
	}
	Tools::comment("Propagation stack exhausted.");
	Tools::decreaseCommentLevel();
	force();
	return false;
}

// Same as processStackHard, but processWatchSoft is called instead.
// Although in the future they will be quite different functions,
// right now the only difference is that processWatchSoft does not add found conflicts to the conflict list,
// but instead saves the (unique) detected conflict in a temporary slot, namely reasons[0].
// This is done for two reasons:
// 1) Making it easier to restore the stack.
// 2) Having a clear conflict candidate when generating the resolution chain.
bool processStackSoft() {
	Tools::comment("Stack soft processing phase.");
	Tools::increaseCommentLevel();
	while(stackCurrent < stackUsed) {
		literal_ = -*stackCurrent;
		Tools::comment("Processing literal " + std::to_string(-literal_) + ". Stack state:   " + Tools::processingStackToString());
		Tools::increaseCommentLevel();
		Tools::comment("Watchlist for literal " + std::to_string(literal_) + ":");
		Tools::watchList(literal_);
		watchIterator_ = 0;
		while(WatchList::processWatchSoft(literal_, watchIterator_)) {}
		if(conflict) {
			Tools::decreaseCommentLevel();
			Tools::decreaseCommentLevel();
			Tools::comment("Stack is in conflicting state.");
			return true;
		} else {
			Tools::decreaseCommentLevel();
			++stackCurrent;
		}
	}
	Tools::comment("Propagation stack exhausted.");
	Tools::decreaseCommentLevel();
	return false;
}

void force() {
	savedCurrent = stackCurrent;
	savedUsed = stackUsed;
	savedConflict = conflict;
}

void restore() {
	while((--stackUsed) >= savedUsed) {
		unassignSoftLiteral(*stackUsed);
	}
	stackUsed = savedUsed;
	stackCurrent = savedCurrent;
	Chain::reasons[0] = 0;
	conflict = savedConflict;
	Tools::comment("Restoring propagation stack. Stack state:   " + Tools::processingStackToString());
}

//
// void print() {
// 	std::cout << "STACK: ";
// 	int* i = stack;
// 	while(i < stackUsed) {
// 		if(i == stackForced) { std::cout<<"||"; }
// 		if(i == stackCurrent) { std::cout<<"#"; }
// 		std::cout << " " << *i;
// 		if(Chain::reasons[*i] == 0) { std::cout << "*"; }
// 		std::cout << " ";
// 		i++;
// 	}
// 	if(i == stackForced) { std::cout<<"||"; }
// 	if(i == stackCurrent) { std::cout<<"#"; }
// 	std::cout << std::endl << std::endl;
// }

}
