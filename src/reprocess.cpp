#include "assignment.hpp"
#include "database.hpp"
#include "watchlist.hpp"
#include "chain.hpp"
#include "reprocess.hpp"
#include "extra.hpp"

namespace Reprocess {

int* reprocessStack;
int* reprocessUsed;
bool* unpropagations;
int* reprocessOld;
int* stackLeft_;
int* stackRight_;
int literal_;
int* clause_;
bool currentFound_;
bool currentInserted_;
int checkLiteral_;
int watchIterator_;

void initialize() {
	Tools::comment("Allocating reprocessing system.");
	reprocessStack = (int*) malloc((Database::noVars + 2) * sizeof(int));
	unpropagations = (bool*) malloc((2 * Database::noVars + 1) * sizeof(bool));
	unpropagations += Database::noVars;
	reprocessUsed = reprocessStack;
	for(int i = 0; i <= Database::noVars; ++i) {
		unpropagations[i] = unpropagations[-i] = false;
	}
}

void deallocate() {
	Tools::comment("Deallocating reprocessing system.");
	free(reprocessStack);
	unpropagations -= Database::noVars;
	free(unpropagations);
}

void recordUnpropagation(int literal) {
	unpropagations[literal] = true;
	*(reprocessUsed++) = literal;
	Assignment::unassignHardLiteral(literal);
}

// Returns true if the clause contains the negation of a literal marked for unpropagation.
// Hence, the literal propagated because of this clause must be unpropagated as well.
// WARNING this function takes a parameter clause by reference which is rendered useless.
bool mustUnpropagate(int* &clause, int literal) {
	while((checkLiteral_ = *(clause++)) != 0) {
		if(unpropagations[-checkLiteral_] && checkLiteral_ != literal) {
			return true;
		}
	}
	return false;
}

void cleanUnpropagations() {
	while(reprocessOld < reprocessUsed) {
		unpropagations[*(reprocessOld++)] = false;
	}
}

// This function does nothing if offset is not a reason clause for an assignment literal.
// If it is, then it removes the further segment of the propagation graph,
// storing it in the unpropagation queue.
void unpropagateClause(long offset) {
	Tools::increaseCommentLevel();
	if((stackRight_ = Chain::findReason(offset)) != 0) {		//We only do something if offset is the reason for an assigned literal.
		Tools::comment("Deactivated clause is a reason for literal " + std::to_string(*stackRight_) + ".");
		reprocessOld = reprocessUsed;				//If this is the case, set a pointer at the beginning of the newly introduced reprocess literals, which will be used for cleaning up the unpropagation switches.
		currentInserted_ = false;									// currentInserted_ is true wwhen the currentStack pointer has been reintroduced (i.e. crossed by the stackLeft_ pointer).
		currentFound_ = (stackRight_ == Assignment::stackCurrent);	// currentFound_ is true wwhen the currentStack marker has been crossed by the stackRight_ pointer.
		recordUnpropagation(*stackRight_);							// Add the literal pointed by stackRight_ to the unpropagation queue, and activate its unpropagation switch.
		stackLeft_ = stackRight_;		//stackLeft_ and stackRight_ will be used to delete unpropagation literals from the stack through the standard trick of using two pointers.
		while((++stackRight_) < Assignment::stackUsed) {			// Now move the stackRight_ pointer all along the propagation queue.
			if(!currentFound_) {											// If the currentStack pointer hasn't been found yet by stackRight_, then check if we have stepped on it.
				currentFound_ = (stackRight_ == Assignment::stackCurrent);	// We remember it so that the next time stackLeft_ is advanced, the stackCurrent pointer is set.
			}
			literal_ = *stackRight_;
			clause_ = Database::getPointer(Chain::reasons[literal_]);		// Retrieve the reason clause for the literal pointed by stackRight_.
			if(mustUnpropagate(clause_, literal_)) {						// WARNING this renders clause_ unusable!
				recordUnpropagation(literal_);								// If the clause must be unpropagated, then add it to the unpropagation queue and activate the unpropagation switch to prevent adding it again.
			} else {														// The literal is only copied to the leftStack_ pointer if it was not scheduled for unpropagation, effectively deleting unpropagated literals from the queue.
				if(!currentInserted_ && currentFound_) {					// In the next leftwards copy after the currentStack pointer has been crossed by rightStack_, move it to the copied literal.
					currentInserted_ = true;
					Assignment::stackCurrent = stackLeft_;
				}
				*stackLeft_ = literal_;										// Copy the right hand literal to the left hand position.
				Assignment::stackPointers[literal_] = stackLeft_++;
			}
		}
		if((!currentInserted_ && currentFound_) || stackRight_ == Assignment::stackUsed) {							// It may happen that you crossed currentStack with the rightStack_ pointer but never had the chance to relocate it. In that case, just do it at the end of the stack.
			Assignment::stackCurrent = stackLeft_;
		}
		Assignment::stackUsed = stackLeft_;
		Tools::reprocessingStack();
		WatchList::unpropagateConflicts();									// Some conflicts may have been rehabilitated as well-respected citizens, so get them out of jail.
		cleanUnpropagations();		//I suspect one can carefully skip cleaning unpropagations,
	} else {						//because the unpropagation stack should be mutually variable-disjoint with the assignment stack.
		Tools::comment("Deactivated clause is not a reason.");
	}
	Tools::decreaseCommentLevel();
}

bool reprocess() {
	Tools::comment("Reprocessing phase.");
	Tools::increaseCommentLevel();
	while((--reprocessUsed) >= reprocessStack) {
		literal_ = *reprocessUsed;
		Tools::comment("Reprocessing literal " + std::to_string(literal_) + ".");
		Tools::increaseCommentLevel();
		if(!Assignment::falsified[-literal_]) {		//If the literal is satisfied again, there is nothing to check.
			if(WatchList::unitUsed[literal_] != 0) {
				Tools::comment("Unit clause detected.");
				if(Assignment::falsified[literal_]) {
					Tools::comment("Conflict detected.");
					Tools::comment("Adding to conflict list.");
					WatchList::addHardConflict(*(WatchList::unitList[literal_]));
					Tools::decreaseCommentLevel();
					Tools::decreaseCommentLevel();
					return true;
				} else {
					Assignment::assignHardLiteral(literal_, *(WatchList::unitList[literal_]));
					Tools::comment("Assigning literal " + std::to_string(literal_) + ". Stack state:   "+ Tools::processingStackToString());
					Tools::decreaseCommentLevel();
				}
			} else {
				Tools::comment("No units on this literal. Checking watchlist:");
				Tools::watchList(literal_);
				watchIterator_ = 0;
				while(WatchList::reprocessWatch(literal_, watchIterator_)) {}
				Tools::decreaseCommentLevel();
				if(Assignment::conflict) {
					Tools::decreaseCommentLevel();
					return true;
				}
			}
		} else {
			Tools::comment("Reprocessed literal is satisfied.");
			Tools::decreaseCommentLevel();
		}
	}
	Tools::comment("Reprocessing stack exhausted.");
	reprocessUsed = reprocessStack;
	Tools::decreaseCommentLevel();
	return false;
}


}
