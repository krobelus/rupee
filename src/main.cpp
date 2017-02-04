#include <iostream>

#include "parser.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "watchlist.hpp"
#include "solver.hpp"
#include "extra.hpp"

using namespace std;

int main(int argc,char* argv[]){
	if(argc < 3) {
		cout << "Usage:" << endl <<
			"\trupee [DIMACS CNF file] [DRAT proof file]" << endl;
	} else {
		for(int argumentCount = 1; argumentCount < argc; ++argumentCount) {
			string str(argv[argumentCount]);
			if(argumentCount == Constants::ArgumentsFilePremise) {
				Parameters::setPremise(str);
			} else if(argumentCount == Constants::ArgumentsFileProof) {
				Parameters::setProof(str);
			}
		}
		Parser::parse();
		Proof::print();
		WatchList::printWatches();
		WatchList::printQueue();
		WatchList::printConflicts();
		Database::print();

		if(Solver::checkProof()) {
			cout << "VERIFIED" << endl;
		} else {
			cout << "INCORRECT" << endl;
		}
		Solver::deallocate();
	}
}
