#include <iostream>
#include <boost/lexical_cast.hpp>

#include "parser.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "extra.hpp"

// using namespace std;

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
	Blablabla::log("Allocating clause storages.");
	Blablabla::increase();
	if(!Parser::allocate(Parser::Buffer)) { return false; }
	if(!Database::allocate()) { return false; }
	if(!Proof::allocate()) { return false; }
	if(!HashTable::allocate()) { return false; }
	Blablabla::decrease();
	return true;
}


bool parse() {
	Blablabla::comment("Parsing CNF instance " + Parameters::pathPremise + " .");
	Blablabla::increase();
	Parser::initialize(Parser::Info);
	if(!Parser::openFile(Parser::Info, Constants::KindOriginal)) { return false; }
	if(!Parser::readHeader(Parser::Info)) { return false; }
	if(!Parser::readClauses(Parser::Info, Parser::Buffer)) { return false; }
	Blablabla::decrease();
	Blablabla::comment("Parsing DRAT proof " + Parameters::pathProof + " .");
	Blablabla::increase();
	if(!Parser::openFile(Constants::KindDerived)) { return false; }
	if(!Parser::readClauses()) { return false; }
	Blablabla::decrease();
	Parser::finish(Parser::Info);
	Parser::deallocate(Parser::Buffer);
	HashTable::deallocate();
	return true;
}


int main(int argc,char* argv[]){
	if(!readArguments(argc, argv)) { return 1; }
	if(!initializeStorage()) { return 1; }
	if(!parse()) { return 1; }


		Parser::parse();
		Proof::print();
		WatchList::printWatches();
		WatchList::printQueue();
		WatchList::printConflicts();
		Database::print();

		if(Solver::checkProof()) {
			std::cout << "VERIFIED" << std::endl;
		} else {
			std::cout << "INCORRECT" << std::endl;
		}
		Solver::deallocate();
	}
	return 0;
}

}

int main(int argc, char* argv[]) {
	return Frontend::main(argc, argv);
}
