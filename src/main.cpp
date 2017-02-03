#include <iostream>

#include "parser.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "watchlist.hpp"
#include "solver.hpp"
#include "extra.hpp"

using namespace std;

int main() {
	cout << "Starting execution." << endl;
	Parameters::setInput("proofs/ph3");
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
