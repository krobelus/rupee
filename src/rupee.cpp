#include <iostream>
#include <boost/lexical_cast.hpp>

#include "structs.hpp"
#include "parser.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "clause.hpp"
#include "hashtable.hpp"
#include "extra.hpp"
#include "checker.hpp"

namespace Frontend {

bool readArguments(int argc, char* argv[]) {
	std::string stringArgument;
	int premiseArgument = Constants::ArgumentsFilePremise;
	for(int i = 1; i < argc; ++i) {
		stringArgument = argv[i];
		std::cout << stringArgument << std::endl;
		if(stringArgument[0] == '-') {
			stringArgument = stringArgument.substr(1);
			if(stringArgument == "help") {
				Blablabla::comment("Usage:");
				Blablabla::comment("\trupee [options] [DIMACS CNF file] [DRAT proof file]");
				Blablabla::comment("Options:");
				Blablabla::comment("\t-help\t\tPrints this help message.");
				Blablabla::comment("\t-failfast\t\tChecks refutation ending in the earliest UP-contradiction.");
				Blablabla::comment("\t-fullcheck\t\tChecks full proof. If backwards-checking is enabled, only dependencies of the last clause in the proof are checked.");
				return false;
			} else if(stringArgument == "failfast") {
				Parameters::failFast = true;
			} else if(stringArgument == "fullcheck") {
				Parameters::failFast = false;
			} else {
				Blablabla::comment("Error: invalid input.");
				Blablabla::comment("\tType rupee -help for more information.");
				return false;
			}
		} else if(premiseArgument == Constants::ArgumentsFilePremise) {
			Parameters::setPremise(stringArgument);
			premiseArgument = Constants::ArgumentsFileProof;
		} else if(premiseArgument == Constants::ArgumentsFileProof) {
			Parameters::setProof(stringArgument);
			premiseArgument = Constants::ArgumentsRest;
		} else {
			Blablabla::comment("Error: invalid input.");
			Blablabla::comment("\tType rupee -help for more information.");
			return false;
		}
	}
	return true;
}

bool initializeStorage() {
	Blablabla::log("Initializing basic data structures.");
	Blablabla::increase();
	if(!Clause::allocate(Objects::Clause)) { return false; }
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
	Blablabla::comment("Deallocating parsing data structures.");
	Blablabla::increase();
	Clause::deallocate(Objects::Clause);
	HashTable::deallocate(Objects::HashTable);
	Parameters::importNoVariables(Objects::Parser);
	Blablabla::decrease();
	Blablabla::logDatabase(Objects::Database);
	Blablabla::logProof(Objects::Proof, Objects::Database);
	return true;
}

bool check() {
	Blablabla::log("Initializing checking data structures.");
	Blablabla::increase();
	if(!Checker::allocate(Objects::Checker)) { return false; }
	Blablabla::decrease();
	if(!Checker::initialize(Objects::Checker, Objects::Proof, Objects::Database)) { return false; }
	if(!Checker::preprocessProof(Objects::Checker, Objects::Proof, Objects::Database, Objects::Result)) { return false; }
	if(Objects::Result) {
		if(!Checker::verifyProof(Objects::Checker, Objects::Proof, Objects::Database, Objects::Result)) { return false; }
		if(Objects::Result) {
			Blablabla::comment("RESULT   correct subrefutation found");
		} else {
			Blablabla::comment("RESULT   correct subrefutation was not found");
		}
	} else {
		Blablabla::comment("RESULT   not a refutation");
	}
	return true;
}

void deallocate() {
	Blablabla::log("Deallocating data structures.");
	Blablabla::increase();
	Database::deallocate(Objects::Database);
	Proof::deallocate(Objects::Proof);
	Checker::deallocate(Objects::Checker);
	Blablabla::decrease();
}

}

int main(int argc, char* argv[]) {
	if(!Frontend::readArguments(argc, argv)) { return 1; }
	if(!Frontend::initializeStorage()) { return 1; }
	if(!Frontend::parse()) { return 1; }
	if(!Frontend::check()) { return 1; }
	Frontend::deallocate();
}
