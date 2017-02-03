#include <iostream>

#include "assignment.hpp"
#include "chain.hpp"
#include "proof.hpp"
#include "reprocess.hpp"
#include "watchlist.hpp"
#include "resolvent.hpp"
#include "database.hpp"
#include "extra.hpp"
#include "solver.hpp"

namespace Solver {

long* proofIterator;
int proofStatus;
long offset_;
int deletion_;
int* clause_;
int literal_;
bool answer_;
int pivot_;
long* candidateIterator_;
long candidateOffset_;

void deallocate() {
	Assignment::deallocate();
	Chain::deallocate();
	Database::deallocate();
	Proof::deallocate();
	Reprocess::deallocate();
	Resolvent::deallocate();
	WatchList::deallocate();
}

bool checkProof() {
	std::cout << "Starting proof checking." << std::endl;
	proofIterator = Proof::proof + Proof::used - 1;
	while(proofIterator >= Proof::proof) {
		Proof::getInstruction(proofIterator, offset_, deletion_);
		if(deletion_ == Constants::KindDeletion) {
			Tools::comment("Deletion instruction: " + Tools::offsetToString(offset_) + ".");
			Tools::increaseCommentLevel();
			Tools::comment("Activating clause.");
			Database::activateClause(offset_);
			Tools::comment("Propagating stack. Stack state:   " + Tools::processingStackToString());
			Assignment::propagateHard();
			Tools::comment("Nothing to check for deletion instructions.");
			Tools::decreaseCommentLevel();
			Tools::comment("");
		} else {
			Tools::comment("Introduction instruction: " + Tools::offsetToString(offset_) + ".");
			Tools::increaseCommentLevel();
			Tools::comment("Deactivating clause.");
			Database::deactivateClause(offset_);
			Tools::comment("Propagating stack.");
			Assignment::propagateHard();
			if(!checkClause(offset_)) {
				Tools::comment("Incorrect inference. Proof is incorrect.");
				Tools::decreaseCommentLevel();
				Tools::comment("");
				return false;
			} else {
				Tools::comment("Introduction instruction " + Tools::offsetToString(offset_) + " was verified.");
				Tools::decreaseCommentLevel();
				Tools::comment("");
			}
		}
		proofIterator--;
	}
	return true;
}

bool checkClause(long offset) {
	clause_ = Database::getPointer(offset);
	if(Database::mustVerify(clause_)) {
		Tools::comment("Clause is marked for checking.");
		return checkRup(clause_) || checkRat(clause_);
	} else {
		Tools::comment("Clause is not marked for checking; skipping.");
		return true;
	}
}

bool checkRup(int* clause) {
	Tools::comment("Checking RUP on clause " + Tools::pointerToString(clause));
	Tools::increaseCommentLevel();
	while((literal_ = -*(clause++)) != 0) {
		if(Assignment::falsified[literal_]) {
			Tools::comment("Literal " + std::to_string(literal_) + " cannot be assumed because it is falsified.");
			Chain::reasons[literal_] = 0;				//This is a "logical hack", but it should work!
			Chain::reasons[0] = Chain::reasons[-literal_];
			Tools::comment("Soft conflict on clause " + Tools::offsetToString(Chain::reasons[0]));
			Assignment::conflict = true;
			Chain::generateChain();
			Chain::markChain();
			Assignment::restore();
			Tools::decreaseCommentLevel();
			return true;
		} else if (!Assignment::falsified[-literal_]) {
			Assignment::assignAssumedLiteral(literal_);
			Tools::comment("Assumming literal " + std::to_string(literal_) + ". Stack state:   " + Tools::processingStackToString());
		}
	}
	if((answer_ = !Assignment::propagateSoft())) {		//YES THIS IS AN ASSIGNMENT, NOT AN EQUALITY CHECK :)
		Tools::comment("RUP check succeeded.");
		Chain::generateChain();
		Chain::markChain();
	} else {
		Tools::comment("RUP check failed.");
	}
	Assignment::restore();
	Tools::decreaseCommentLevel();
	return answer_;
}

bool checkRat(int* clause) {
	Tools::comment("Checking RAT.");
	pivot_ = clause[Constants::PivotCellDatabase];
	if(pivot_ == 0) {
		return false;
	} else {
		Resolvent::findResolutionCandidates(pivot_);
		candidateIterator_ = Resolvent::candidatesQueue;
		Resolvent::setupResolutionSystem(clause, pivot_);
		while(Resolvent::computeNextResolvent(candidateIterator_)) {
			if(checkRup(Resolvent::resolventClause)) {
				return false;
			}
		}
		Resolvent::cleanResolvent();
		return true;
	}
}

void setStatus(int status) {
	if(proofStatus == Constants::NoContradiction) {
		proofStatus = status;
	}
}

}
