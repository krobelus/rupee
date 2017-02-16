#include <iostream>
#include <boost/lexical_cast.hpp>

#include "structs.hpp"
#include "parser.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "clause.hpp"
#include "hashtable.hpp"
#include "trail.hpp"
#include "extra.hpp"
#include "watchlist.hpp"

namespace Frontend {

bool readArguments(int argc, char* argv[]) {
	std::string stringArgument;
	int premiseArgument = Constants::ArgumentsFilePremise;
	bool finishArguments = false;
	for(int i = 1; i < argc && !finishArguments; ++i) {
		stringArgument = std::string(argv[i]);
		if(stringArgument[0] == '-') {
			stringArgument = stringArgument.substr(1);
			if(stringArgument == "help") {
				premiseArgument = Constants::ArgumentsHelp;
				finishArguments = true;
			} else {
				premiseArgument = Constants::ArgumentsError;
				finishArguments = true;
			}
		} else {
			if(premiseArgument == Constants::ArgumentsFilePremise) {
				Parameters::setPremise(stringArgument);
				premiseArgument = Constants::ArgumentsFileProof;
			} else if (premiseArgument == Constants::ArgumentsFileProof) {
				Parameters::setProof(stringArgument);
				premiseArgument = Constants::ArgumentsRest;
			} else {
				premiseArgument = Constants::ArgumentsError;
				finishArguments = true;
			}
		}
	}
	if(premiseArgument != Constants::ArgumentsRest && premiseArgument != Constants::ArgumentsHelp) {
		premiseArgument = Constants::ArgumentsError;
	}
	if(premiseArgument == Constants::ArgumentsError) {
		Blablabla::comment("Error: invalid input.");
		Blablabla::comment("\tType rupee -help for more information.");
		return false;
	} else if(premiseArgument == Constants::ArgumentsHelp) {
		Blablabla::comment("Usage:");
		Blablabla::comment("\trupee [DIMACS CNF file] [DRAT proof file]");
		return false;
	} else {
		return true;
	}
}

bool initializeStorage() {
	Blablabla::log("Initializing basic data structures.");
	Blablabla::increase();
	if(!Clause::allocateBuffer(Objects::Clause)) { return false; }
	if(!HashTable::allocate(Objects::HashTable)) { return false; }
	if(!Database::allocate(Objects::Database)) { return false; }
	if(!Proof::allocate(Objects::Proof)) { return false; }
	Blablabla::decrease();
	return true;
}

bool parse() {
	Blablabla::comment("Parsing CNF instance " + Parameters::pathPremise + " .");
	Blablabla::increase();
	Parser::initialize(Objects::Parser);
	if(!Parser::openFile(Objects::Parser, Constants::FilePremise)) { return false; }
	if(!Parser::readHeader(Objects::Parser)) { return false; }
	if(!Parser::readClauses(Objects::Parser, Objects::Clause, Objects::HashTable, Objects::Database, Objects::Proof)) { return false; }
	Blablabla::decrease();
	Blablabla::comment("Parsing DRAT proof " + Parameters::pathProof + " .");
	Blablabla::increase();
	if(!Parser::openFile(Objects::Parser, Constants::FileProof)) { return false; }
	if(!Parser::readClauses(Objects::Parser, Objects::Clause, Objects::HashTable, Objects::Database, Objects::Proof)) { return false; }
	Blablabla::decrease();
	Blablabla::comment("Freeing parsing data structures.");
	Blablabla::increase();
	Clause::resetBuffer(Objects::Clause);
	HashTable::deallocate(Objects::HashTable);
	Parameters::importNoVariables(Objects::Parser);
	Blablabla::decrease();
	Database::log(Objects::Database);
	Proof::log(Objects::Proof, Objects::Database);
	return true;
}

bool initializeAuxiliary() {
	if(!Trail::allocate(Objects::Trail)) { return false; }
	if(!WatchList::allocate(Objects::WatchList)) { return false; }
	return true;
}

bool preprocessPremisses() {
	Proof::firstPremise(Objects::Proof, Objects::ProofIterator);
	while(Proof::getPremise(Objects::Proof, Objects::Database, Objects::ProofIterator)) {
		Database::setPremiseFlags(Objects::ProofIterator.pointer);
		if(!WatchList::insertWatches(Objects::WatchList, Objects::ProofIterator)) { return false; }
	}
	return true;
}

bool preprocessProof() {
	Proof::firstInstruction(Objects::Proof, Objects::ProofIterator);
	while(Proof::getPremise(Objects::Proof, Objects::Database, Objects::ProofIterator)) {
		if(Objects::ProofIterator.kind == Constants::InstructionIntroduction) {
			Database::setIntroductionFlags(Objects::ProofIterator.pointer);
			if(!WatchList::insertWatches(Objects::WatchList, Objects::ProofIterator)) { return false; }
		} else {
			Database::setDeletionFlags(Objects::ProofIterator.pointer);
			if(!WatchList::removeWatches(Objects::WatchList, Objects::ProofIterator)) { return false; }
		}
	}
	return true;
}

// int main(int argc,char* argv[]){
// 	if(!readArguments(argc, argv)) { return 1; }
// 	if(!initializeStorage()) { return 1; }
// 	if(!parse()) { return 1; }
//
//
// 		Parser::parse();
// 		Proof::print();
// 		WatchList::printWatches();
// 		WatchList::printQueue();
// 		WatchList::printConflicts();
// 		Database::print();
//
// 		if(Solver::checkProof()) {
// 			std::cout << "VERIFIED" << std::endl;
// 		} else {
// 			std::cout << "INCORRECT" << std::endl;
// 		}
// 		Solver::deallocate();
// 	}
// 	return 0;
// }

}

int main(int argc, char* argv[]) {
	if(!Frontend::readArguments(argc, argv)) { return 1; }
	if(!Frontend::initializeStorage()) { return 1; }
	if(!Frontend::parse()) { return 1; }
	if(!Frontend::initializeAuxiliary()) { return 1; }
	return 0;
}
