#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "hashtable.hpp"
#include "assignment.hpp"
#include "solver.hpp"
#include "chain.hpp"
#include "reprocess.hpp"
#include "resolvent.hpp"
#include "watchlist.hpp"
#include "parser.hpp"

namespace Parser {

int* buffer_;
int buffersize_;
int length_;
int phase_;
int file_;
int deletion_;
int literal_;
long offset_;
unsigned int hashValue_;
std::string line_;
std::ifstream input_;
boost::smatch result_;

void parse() {
	std::cout << "Parsing files." << std::endl;
	findMaximumVariable();
	initialize();
	readClauses();
	deallocate();
}

void findMaximumVariable() {
	Database::noVars = 0;
	phase_ = Constants::ParsingPhasePreprocessPremises;
	input_.open(Parameters::pathPremise);
	while(phase_ < Constants::ParsingPhaseInitialize) {
		if(input_.is_open()) {
			if(phase_ == Constants::ParsingPhasePreprocessPremises) {
				skipToHeader();
			}
			while(input_.good()) {
				getline(input_, line_);
				while(boost::regex_search(line_, result_, literalRegex)) {
					if(Database::noVars < abs(boost::lexical_cast<int>(result_[1]))) {
						Database::noVars = abs(boost::lexical_cast<int>(result_[1]));
					}
					line_ = result_[2];
				}
			}
		} else {
			if(phase_ == Constants::ParsingPhasePreprocessPremises) {
				std::cout << "ERROR: Couldn't open premise file." << std::endl;
			} else {
				std::cout << "ERROR: Couldn't open proof file." << std::endl;
			}
		}
		input_.close();
		if(phase_ == Constants::ParsingPhasePreprocessPremises) {
			input_.open(Parameters::pathProof);
		}
		phase_++;
	}
	std::cout << Database::noVars << " variables detected." << std::endl;
}

void skipToHeader() {
	while(input_.good()) {
		getline(input_, line_);
		if(boost::regex_search(line_, result_, headerRegex)) {
			return;
		}
	}
	if(phase_ == Constants::ParsingPhasePreprocessPremises) {
		std::cout << "ERROR: No header was found in premise file" << std::endl;
	}
}

void initialize() {
	std::cout << "Allocating data structures." << std::endl;
	buffersize_ = Database::noVars * 2;
	buffer_ = (int*) malloc(buffersize_ * sizeof(int));
	Database::allocate();
	HashTable::allocate();
	Proof::allocate();
	Assignment::initialize();
	WatchList::initialize();
	Chain::initialize();
	Reprocess::initialize();
	Resolvent::initialize();
	phase_++;
}

void readClauses() {
	Solver::proofStatus = Constants::NoContradiction;
	while(phase_ < Constants::ParsingDeallocate) {
		if(phase_ == Constants::ParsingPhaseReadPremises) {		///Prepares to read clauses in the corresponding file
			input_.open(Parameters::pathPremise);
			skipToHeader();
			file_ = Constants::KindOriginal;
		} else {
			input_.open(Parameters::pathProof);
			file_ = Constants::KindDerived;
		}
		if(input_.is_open()) {				///If the input file is not open, something went wrong and errors are issued.
			while(input_.good()) {			/// For every line in the file after headers and such...
				getline(input_, line_);
				if(!line_.empty()) {
					if(parseClause()) {			///... try to parse it as a clause
						processClause();			/// If I am successful, I process it
						Tools::decreaseCommentLevel();
					} else {						/// Otherwise an error is issued
						if(phase_ == Constants::ParsingPhasePreprocessPremises) {
							std::cout << "ERROR: Couldn't parse premise file." << std::endl;
						} else {
							std::cout << "ERROR: Couldn't parse proof file." << std::endl;
						}
					}
				}
			}
		} else {
			if(phase_ == Constants::ParsingPhasePreprocessPremises) {
				std::cout << "ERROR: Couldn't open premise file." << std::endl;
			} else {
				std::cout << "ERROR: Couldn't open proof file." << std::endl;
			}
		}
		input_.close();
		phase_++;
	}
	if(Solver::proofStatus == Constants::ProofContradiction) {
		std::cout << "Detected contradiction in proof." << std::endl;
	} else if(Solver::proofStatus == Constants::PremiseContradiction) {
		std::cout << "Detected contradiction in premises." << std::endl;
	} else if(Solver::proofStatus == Constants::NoContradiction) {
		std::cout << "WARNING: no contradiction in premises or proof." << std::endl;
	}
}

bool parseClause() {
	length_ = 0;
	if(boost::regex_search(line_, result_, deletionRegex)) {
		deletion_ = Constants::KindDeletion;
	} else {
		deletion_ = Constants::KindIntroduction;
	}
	while(boost::regex_search(line_, result_, literalRegex)) {
		literal_ = boost::lexical_cast<int>(result_[1]);
		if(literal_ == 0) {
			buffer_[length_] = 0;
			std::string str = "Read instruction ";
			if(deletion_ == Constants::KindDeletion) {
				str = str + "d ";
			}
			str = str + Tools::pointerToString(buffer_);
			Tools::comment(str);
			Tools::increaseCommentLevel();
			return true;
		} else {
			buffer_[length_++] = literal_;
			line_ = result_[2];
		}
	}
	return false;
}


void processClause() {
	literal_ = buffer_[0];								/// Store the pivot, sort the clause and obtain the hash.
	Tools::sortClause(buffer_, length_);				/// WARNING I think it's not good for performance to sort it before choosing the watched literals...
	hashValue_ = HashTable::getHash(buffer_);
	if(deletion_ == Constants::KindIntroduction) {
		offset_ = Database::addClause(buffer_, length_, literal_, file_ | Constants::KindSkip | Constants::KindTemporal);
		HashTable::addClause(hashValue_, offset_);
		Database::activateClause(offset_);
		if(file_ == Constants::KindOriginal) {
			Database::setFlag(offset_, Constants::PersistencyFlagDatabase, Constants::KindPermanent);
		}
	} else {
		offset_ = HashTable::matchAndRemoveClause(buffer_,length_, hashValue_);
		if(offset_ < 0) {
			std::cout << "WARNING: proof deletes a missing clause." << std::endl;
			offset_ = Database::addClause(buffer_, length_, literal_, deletion_ | file_ | Constants::KindSkip | Constants::KindTemporal);
		}
		Database::deactivateClause(offset_);
		Database::setFlag(offset_, Constants::PersistencyFlagDatabase, Constants::KindTemporal);
	}
	if(file_ == Constants::KindDerived) {
		Proof::storeInstruction(offset_, deletion_);
	}
	if(literal_ == 0) {
		if(file_ == Constants::KindOriginal) {
			Solver::setStatus(Constants::PremiseContradiction);
		} else {
			Solver::setStatus(Constants::ProofContradiction);
			Database::setFlag(offset_, Constants::VerificationFlagDatabase, Constants::KindVerify);
		}
	}
}

void deallocate() {
	free(buffer_);
	HashTable::deallocate();
}

}
