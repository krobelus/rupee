#include <iostream>

#include "extra.hpp"
#include "database.hpp"
#include "assignment.hpp"
#include "chain.hpp"
#include "reprocess.hpp"
#include "watchlist.hpp"

namespace WatchList {

long** watchList;
int* watchUsed;
int* watchMax;
long** unitList;
int* unitUsed;
int* unitMax;
long* queueList;
int queueUsed;
int queueMax;
long* conflictList;
int conflictUsed;
int conflictMax;
long offset_;
int* clause_;
int* rowlit_;
int* watchedlit_;
int* replaceWatch_;
int* firstWatch_;
int literal_;
int tempLiteral_;
bool firstwatchfound_;
int watchlistIterator_;
bool checkpropagation_;
bool rowpropagation_;
long* removeIterator_;
int unpLiteral_;
long unpOffset_;
int* unpClause_;
bool unpDone_;
int unpConflictIterator_;

void initialize() {
	Tools::comment("Allocating watchlists.");
	unitList = (long**) malloc((2 * Database::noVars + 1) * sizeof(long*));
	unitList += Database::noVars;
	unitMax = (int*) malloc((2 * Database::noVars + 1) * sizeof(int));
	unitMax += Database::noVars;
	unitUsed = (int*) malloc((2 * Database::noVars + 1) * sizeof(int));
	unitUsed += Database::noVars;

	watchList = (long**) malloc((2 * Database::noVars + 1) * sizeof(long*));
	watchList += Database::noVars;
	watchMax = (int*) malloc((2 * Database::noVars + 1) * sizeof(int));
	watchMax += Database::noVars;
	watchUsed = (int*) malloc((2 * Database::noVars + 1) * sizeof(int));
	watchUsed += Database::noVars;

	for(int i = -Database::noVars; i <= Database::noVars; ++i) {
		watchMax[i] = unitMax[i] = Constants::SmallAllocation;
		watchUsed[i] = unitUsed[i] = 0;
		watchList[i] = (long*) malloc(watchMax[i] * sizeof(long));
		unitList[i] = (long*) malloc(unitMax[i] * sizeof(long));
		watchList[i][0] = unitList[i][0] = 0;
	}

	conflictMax = queueMax = Constants::SmallAllocation;
	conflictUsed = queueUsed = 0;
	conflictList = (long*) malloc(conflictMax * sizeof(long));
	queueList = (long*) malloc(queueMax * sizeof(long));
	conflictList[0] = queueList[0] = 0;
}

void deallocate() {
	Tools::comment("Deallocating watchlists.");
	free(watchList[0]);
	free(unitList[0]);
	for(int i = 1; i <= Database::noVars; ++i) {
		free(watchList[i]);
		free(watchList[-i]);
		free(unitList[-i]);
		free(unitList[i]);
	}
	watchList -= Database::noVars;
	watchMax -= Database::noVars;
	watchUsed -= Database::noVars;
	unitList -= Database::noVars;
	unitMax -= Database::noVars;
	unitUsed -= Database::noVars;
	free(conflictList);
	free(queueList);
	free(watchList);
	free(watchMax);
	free(watchUsed);
	free(unitList);
	free(unitMax);
	free(unitUsed);
}

void addToList(long offset, long* &list, int &used, int &max) {
	if(used >= max - 1) {
		Tools::comment("Reallocating watchlist.");
		max *= 2;
		list = (long*) realloc(list, max * sizeof(long));
	}
	list[used++] = offset;
	list[used] = 0;
}

void removeFromList(long offset, long* &list, int &used) {
	for(removeIterator_ = list; (offset_ = *removeIterator_) != 0; ++removeIterator_) {
		if(offset_ == offset) {
			*removeIterator_ = list[--used];
			list[used] = 0;
			return;
		}
	}
}

void removeInList(int pos, long* &list, int &used) {
	list[pos] = list[--used];
	list[used] = 0;
}

void addHardConflict(long offset) {
	addToList(offset, conflictList, conflictUsed, conflictMax);
	Assignment::conflict = true;
}

void addSoftConflict(long offset) {
	Chain::reasons[0] = offset;
	Assignment::conflict = true;
}

void addUnit(long offset, int lit) {
	addToList(offset, unitList[lit], unitUsed[lit], unitMax[lit]);
	addToList(offset, queueList, queueUsed, queueMax);
}

void addLargeClause(long offset, int* clause) {
	tempLiteral_ = *(clause++);
	addToList(offset, watchList[tempLiteral_], watchUsed[tempLiteral_], watchMax[tempLiteral_]);
	tempLiteral_ = *clause;
	addToList(offset, watchList[tempLiteral_], watchUsed[tempLiteral_], watchMax[tempLiteral_]);
}

void introduceClause(long offset) {
	clause_ = watchedlit_ =  Database::getPointer(offset);	//clause_ will always point to the first literal.
	if((literal_ = *(watchedlit_++)) == 0) {
		Tools::comment("Empty clause detected.");
		Tools::comment("Adding to conflict list.");
		addHardConflict(offset);
	} else if(*(watchedlit_--) == 0) {
		Tools::comment("Unit clause detected.");
		Tools::comment("Adding to unit list.");
		Tools::comment("Adding to unit queue.");
		addUnit(offset, literal_);
	} else {							// Otherwise it is a large clause.
		Tools::comment("Large clause detected.");
		firstwatchfound_ = false;		// firstwatchfound_ only gets true if an unassigned literal has been found already.
		while(true) {					// At this point, both clause_ and watchedlit_ point to the first literal; and literal_ holds the value of the first literal.
			if(literal_ == 0) {				//If the clause has been exhausted, then...
				addLargeClause(offset, clause_);	// The second parameter needs to point to the second literal.
				if(firstwatchfound_) {				// Either there is only one unassigned literal, namely clause_[0], and no satisfied literals,
					Tools::comment("Clause in triggered position.");
					Tools::comment("Adding to watchlists " + std::to_string(clause_[0]) + " and " + std::to_string(clause_[1]) + ".");
					Tools::comment("Propagating literal " + std::to_string(clause_[0]));
					Assignment::assignHardLiteral(*clause_, offset);		//in which case the clause is in triggered position and the first literal must be propagated;
				} else {							// or there are no unassigned or satisfied literals,
					Tools::comment("Clause in falsified position.");
					Tools::comment("Adding to conflict list.");
					addHardConflict(offset);			// in which case the clause is in falsified position and a conflict must be declared.
				}									//In both cases, the invariant can be kept by choosing clause_[0] and clause_[1] as watched literals, so the addLargeClause call above is justified.
				return;
			} else if(Assignment::falsified[-literal_]) {	//If the clause was not exhausted, I am reading a literal. Assume it is satisfied.
				*watchedlit_ = *clause_;						//Then I put it in the first position, and I don't care about the other literal.
				*clause_ = literal_;
				addLargeClause(offset, clause_);			//Insert the watches and exit.
				Tools::comment("Clause in satisfied position.");
				Tools::comment("Adding to watchlists " + std::to_string(clause_[0]) + " and " + std::to_string(clause_[1]) + ".");
				return;
			} else if(!Assignment::falsified[literal_]) {	//If the literal was unassigned, then...
				if(firstwatchfound_) {					//either it is the second unassigned literal I find...
					*watchedlit_ = *(++clause_);		//so I put it in the second position in the clause and insert the watches reporting success;
					*(clause_--) = literal_;
					Tools::comment("Clause in idle position.");
					Tools::comment("Adding to watchlists " + std::to_string(clause_[0]) + " and " + std::to_string(clause_[1]) + ".");
					addLargeClause(offset, clause_);
					return;
				} else {			//or it is just the first unassigned literal I find...
					*watchedlit_ = *clause_;		//so I put it in the first position in the clause, and remember that I found one unassigned literal
					*clause_ = literal_;
					firstwatchfound_ = true;
					literal_ = *(++watchedlit_);
				}
			} else {
				literal_ = *(++watchedlit_);
			}
		}
	}
}

void removeClause(long offset) {
	clause_ = Database::getPointer(offset);
	Reprocess::unpropagateClause(offset);
	removeFromList(offset, conflictList, conflictUsed);
	if(conflictUsed == 0) {
		Assignment::conflict = false;
	}
	if((literal_ = *(clause_++)) != 0) {
		if((tempLiteral_ = *clause_) == 0) {
			removeFromList(offset, unitList[literal_], unitUsed[literal_]);
			removeFromList(offset, queueList, queueUsed);
		} else {
			removeFromList(offset, watchList[literal_], watchUsed[literal_]);
			removeFromList(offset, watchList[tempLiteral_], watchUsed[tempLiteral_]);
		}
	}
}

void unpropagateConflicts() {
	unpConflictIterator_ = 0;
	while((unpOffset_ = conflictList[unpConflictIterator_]) != 0) {
		unpClause_ = Database::getPointer(unpOffset_);
		unpDone_ = false;
		while(!unpDone_) {
			if((unpLiteral_ = *(unpClause_++)) != 0) {
				if(Reprocess::unpropagations[-unpLiteral_]) {
					unpDone_ = true;
					removeInList(unpConflictIterator_, conflictList, conflictUsed);
				}
			} else {
				unpDone_ = true;
				++unpConflictIterator_;
			}
		}
	}
	if(conflictUsed == 0) {
		Assignment::conflict = false;
	}
}

bool processUnits() {
	Tools::comment("Unit processing phase.");
	Tools::increaseCommentLevel();
	while(queueUsed != 0) {
		offset_ = *queueList;
		literal_ = *(Database::getPointer(offset_));
		Tools::comment("Processing unit " + std::to_string(literal_));
		Tools::increaseCommentLevel();
		if(Assignment::falsified[literal_]) {
			Tools::comment("Conflict detected.");
			Tools::comment("Adding to conflict list.");
			addHardConflict(offset_);
			Tools::decreaseCommentLevel();
			Tools::decreaseCommentLevel();
			return true;
		} else if(!Assignment::falsified[-literal_]) {
			Assignment::assignHardLiteral(literal_, offset_);
			removeInList(0, queueList, queueUsed);
			Tools::comment("Assigning literal " + std::to_string(literal_) + ". Stack state:   "+ Tools::processingStackToString());
			Tools::decreaseCommentLevel();
		} else {
			removeInList(0, queueList, queueUsed);
			Tools::comment("Clause is satisfied.");
			Tools::decreaseCommentLevel();
		}
	}
	Tools::comment("Unit stack exhausted.");
	Tools::decreaseCommentLevel();
	return false;
}

// This assumes that the row literal is watched and falsified.
bool processWatchHard(int row, int &position) {
	offset_ = watchList[row][position];
	if(offset_ == 0) {		//If the watchlist is exhausted, then stop the propagation.
		Tools::comment("Watchlist exhausted.");
		return false;
	} else {
		watchedlit_ = Database::getPointer(offset_);	//Otherwise, there is some clause remaining in the watchlist.
		Tools::comment("Processing watched clause " + Tools::pointerToString(watchedlit_));
		Tools::increaseCommentLevel();
		clause_ = watchedlit_++;			//Set clause_ pointing to the first literal and watchedlit_ to the second (remember, it's a large clause!).
		literal_ = *clause_;				//Store the values of the watched literals in literal_ and tempLiteral_ respectively.
		tempLiteral_ = *watchedlit_;
		if(Assignment::falsified[-literal_] || Assignment::falsified[-tempLiteral_]) {
			++position;						//If the clause is satisfied by any of those literals, then process next clause.
			Tools::comment("Clause is satisfied by the other watched literal.");
			Tools::decreaseCommentLevel();
			return true;
		} else {							//Otherwise, the processed literal is known to be falsified, so:
			if(row == literal_) {					// 1) Make replaceWatch_ point to the processed literal in the clause
				replaceWatch_ = clause_;			// 2) Make tempLiteral_ hold the value of other watched literal.
			} else {
				replaceWatch_ = watchedlit_;
				tempLiteral_ = literal_;
			}
			while((literal_ = *(++watchedlit_)) != 0) {
				if(!Assignment::falsified[literal_]) {						// If any non-watched literal is satisfied or unassigned,
					*watchedlit_ = row;										// then swap the processed literal and the non-falsified literal.
					*replaceWatch_ = literal_;
					removeInList(position, watchList[row], watchUsed[row]);	// remove this clause from the watchlist of the processed literal
					addToList(offset_, watchList[literal_], watchUsed[literal_], watchMax[literal_]);
					Tools::comment("Replacing watches. Watching clause on literals " + std::to_string(clause_[0]) + " and " + std::to_string(clause_[1]) + ".");
					Tools::decreaseCommentLevel();
					return true;											// then go on to process the next watched clause.
				}
			}													// Otherwise, all non-watched literals are falsified
			if(Assignment::falsified[tempLiteral_]) {				// If the other watched literal is also falsified, then the clause is falsified
				Tools::comment("Clause is falsified.");
				Tools::comment("Adding to conflict list.");
				addHardConflict(offset_);					// and a conflict must be declared.
				Tools::decreaseCommentLevel();
				return false;										// Exit and stop processing.
			} else {												// Or else, the clause is triggered, so the literal must be propagated
				Assignment::assignHardLiteral(tempLiteral_,offset_);
				++position;											// Go on to process the next watched clause.
				Tools::comment("Clause is triggered.");
				Tools::comment("Assigning literal " + std::to_string(tempLiteral_) + ". Stack state:   " + Tools::processingStackToString());
				Tools::decreaseCommentLevel();
				return true;
			}
		}
	}
}

bool processWatchSoft(int row, int &position) {
	offset_ = watchList[row][position];
	if(offset_ == 0) {
		Tools::comment("Watchlist exhausted.");
		return false;
	} else {
		watchedlit_ = Database::getPointer(offset_);
		Tools::comment("Processing watched clause " + Tools::pointerToString(watchedlit_));
		Tools::increaseCommentLevel();
		clause_ = watchedlit_++;
		literal_ = *clause_;
		tempLiteral_ = *watchedlit_;
		if(Assignment::falsified[-literal_] || Assignment::falsified[-tempLiteral_]) {
			++position;
			Tools::comment("Clause is satisfied by the other watched literal.");
			Tools::decreaseCommentLevel();
			return true;
		} else {
			if(row == literal_) {
				replaceWatch_ = clause_;
			} else {
				replaceWatch_ = watchedlit_;
				tempLiteral_ = literal_;
			}
			while((literal_ = *(++watchedlit_)) != 0) {
				if(!Assignment::falsified[literal_]) {
					*watchedlit_ = row;
					*replaceWatch_ = literal_;
					removeInList(position, watchList[row], watchUsed[row]);
					addToList(offset_, watchList[literal_], watchUsed[literal_], watchMax[literal_]);
					Tools::comment("Replacing watches. Watching clause on literals " + std::to_string(clause_[0]) + " and " + std::to_string(clause_[1]) + ".");
					Tools::decreaseCommentLevel();
					return true;
				}
			}
			if(Assignment::falsified[tempLiteral_]) {
				addSoftConflict(offset_);		//Conflicts are soflty declared
				Tools::comment("Clause is falsified.");
				Tools::comment("Adding to conflict list.");
				Tools::decreaseCommentLevel();
				return false;
			} else {
				Assignment::assignSoftLiteral(tempLiteral_,offset_);	//Literals are softly assigned (although currently no difference is implemented).
				++position;
				Tools::comment("Clause is triggered.");
				Tools::comment("Assigning literal " + std::to_string(tempLiteral_) + ". Stack state:   " + Tools::processingStackToString());
				Tools::decreaseCommentLevel();
				return true;
			}
		}
	}
}


//Take care in here... you need to remove the watches!
bool reprocessWatch(int row, int &position) {
	offset_ = watchList[row][position];
	if(offset_ == 0) {
		Tools::comment("Watchlist exhausted.");
		return false;
	} else if(Assignment::falsified[-row]) {
		Tools::comment("Reprocessed literal is satisfied.");
		return false;
	} else {
		clause_ = Database::getPointer(offset_);
		Tools::comment("Processing watched clause " + Tools::pointerToString(clause_));
		Tools::increaseCommentLevel();
		if(clause_[0] == row) {
			watchedlit_ = rowlit_ = clause_;
			tempLiteral_ = *(++watchedlit_);
		} else {
			watchedlit_ = rowlit_ = clause_;
			tempLiteral_ = *(rowlit_++);
		}		// At this point, rowlit_ points to the row literal; watchedlit_ points to the other watched literal; and tempLiteral_ holds the other watch's value.
		if(Assignment::falsified[-tempLiteral_]) {
			Tools::comment("Clause is satisfied in the other watched literal.");
			Tools::decreaseCommentLevel();
			++position;
			return true;
		} else {
			if(Assignment::falsified[row]) {
				if(Assignment::falsified[tempLiteral_]) {
					checkpropagation_ = false;
					firstwatchfound_ = false;
					replaceWatch_ = rowlit_;		//I prefer to replace first the row watch, since it's easier to delete if needed.
				} else {
					checkpropagation_ = true;		//Case 1
					rowpropagation_ = true;
					replaceWatch_ = clause_;
				}
			} else {
				if(Assignment::falsified[tempLiteral_]) {	//Case 2
					checkpropagation_ = true;
					rowpropagation_ = false;
					replaceWatch_  = watchedlit_;
				} else {
					Tools::comment("Both watches are unassigned.");
					Tools::decreaseCommentLevel();
					++position;
					return true;
				}
			}
			++clause_;				// At this point clause_ points at the second literal.
			while((literal_ = *(++clause_)) != 0) {
				if(Assignment::falsified[-literal_]) {
					*rowlit_ = literal_;
					*clause_ = row;
					removeInList(position, watchList[row], watchUsed[row]);
					addToList(offset_, watchList[literal_], watchUsed[literal_], watchMax[literal_]);
					Tools::comment("Found a satisfied literal. Watching clause on literals " + std::to_string(tempLiteral_) + " and " + std::to_string(literal_) + ".");
					Tools::decreaseCommentLevel();
					return true;
				} else if(!Assignment::falsified[literal_]) {
					if(checkpropagation_) {
						if(rowpropagation_) {
							*rowlit_ = literal_;
							*clause_ = row;
							removeInList(position, watchList[row], watchUsed[row]);
						} else {
							*watchedlit_ = literal_;
							*clause_ = tempLiteral_;
							removeFromList(offset_, watchList[tempLiteral_], watchUsed[tempLiteral_]);
						}
						addToList(offset_, watchList[literal_], watchUsed[literal_], watchMax[literal_]);
						Tools::comment("Replacing watches. Watching clause on literals " + std::to_string(tempLiteral_) + " and " + std::to_string(literal_) + ".");
						Tools::decreaseCommentLevel();
						return true;
					} else {
						if(firstwatchfound_) {
							*rowlit_ = *firstWatch_;
							*firstWatch_ = row;
							*watchedlit_ = literal_;
							*clause_ = tempLiteral_;
							removeInList(position, watchList[row], watchUsed[row]);
							removeFromList(offset_, watchList[tempLiteral_], watchUsed[tempLiteral_]);	// remove from the other watch's watchlist
							tempLiteral_ = *firstWatch_;
							addToList(offset_, watchList[tempLiteral_], watchUsed[tempLiteral_], watchMax[tempLiteral_]);
							addToList(offset_, watchList[literal_], watchUsed[literal_], watchMax[literal_]);
							Tools::comment("Replacing watches. Watching clause on literals " + std::to_string(tempLiteral_) + " and " + std::to_string(literal_) + ".");
							Tools::decreaseCommentLevel();
							return true;
						} else {
							firstwatchfound_ = true;
							firstWatch_ = clause_;
						}
					}
				}
			}
			if(checkpropagation_) {
				if(rowpropagation_) {
					Assignment::assignHardLiteral(tempLiteral_, offset_);
					Tools::comment("Clause is triggered.");
					Tools::comment("Assigning literal " + std::to_string(tempLiteral_) + ". Stack state:   " + Tools::processingStackToString());
					Tools::decreaseCommentLevel();
					++position;
					return true;
				} else {
					Assignment::assignHardLiteral(row, offset_);
					Tools::comment("Clause is triggered.");
					Tools::comment("Assigning literal " + std::to_string(row) + ". Stack state:   " + Tools::processingStackToString());
					Tools::decreaseCommentLevel();
					Tools::comment("Reprocessed literal is satisfied.");
					return false;
				}
			} else {
				if(firstwatchfound_) {
					removeInList(position, watchList[row], watchUsed[row]);
					*rowlit_ = tempLiteral_ = *firstWatch_;
					*firstWatch_ = row;
					Assignment::assignHardLiteral(tempLiteral_, offset_);
					addToList(offset_, watchList[tempLiteral_], watchUsed[tempLiteral_], watchMax[tempLiteral_]);
					Tools::comment("Clause is triggered.");
					Tools::comment("Assigning literal " + std::to_string(tempLiteral_) + ". Stack state:   " + Tools::processingStackToString());
					Tools::decreaseCommentLevel();
					return true;
				} else {
					Tools::comment("Clause is falsified.");
					Tools::comment("Adding to conflict list.");
					Tools::decreaseCommentLevel();
					addHardConflict(offset_);
					return false;
				}
			}
		}
	}
}

void printWatches() {
	Tools::comment("WATCHLIST:");
	Tools::increaseCommentLevel();
	for(int i = 1; i <= Database::noVars; ++i) {
		Tools::comment("Watchlist for literal " + std::to_string(i) + ":");
		Tools::increaseCommentLevel();
		for(int j = 0; j < unitUsed[i]; ++j) {
			Tools::comment(Tools::offsetToString(unitList[i][j]) + " [unit]");
		}
		for(int j = 0; j < watchUsed[i]; ++j) {
			Tools::comment(Tools::offsetToString(watchList[i][j]));
		}
		Tools::decreaseCommentLevel();
		i *= -1;
		Tools::comment("Watchlist for literal " + std::to_string(i) + ":");
		Tools::increaseCommentLevel();
		for(int j = 0; j < unitUsed[i]; ++j) {
			Tools::comment(Tools::offsetToString(unitList[i][j]) + " [unit]");
		}
		for(int j = 0; j < watchUsed[i]; ++j) {
			Tools::comment(Tools::offsetToString(watchList[i][j]));
		}
		Tools::decreaseCommentLevel();
		i *= -1;
	}
	Tools::comment("----------------------------------");
	Tools::comment("");
	Tools::decreaseCommentLevel();
}

void printConflicts() {
	Tools::comment("CONFLICT LIST:");
	Tools::increaseCommentLevel();
	for(int i = 0; i < conflictUsed; ++i) {
		Tools::comment(Tools::offsetToString(conflictList[i]));
	}
	Tools::comment("----------------------------------");
	Tools::comment("");
	Tools::decreaseCommentLevel();
}


void printQueue() {
	Tools::comment("UNIT QUEUE:");
	Tools::increaseCommentLevel();
	for(int i = 0; i < queueUsed; ++i) {
		Tools::comment(Tools::offsetToString(queueList[i]));
	}
	Tools::comment("----------------------------------");
	Tools::comment("");
	Tools::decreaseCommentLevel();
}

// void print() {
// 	std::cout << "WATCHLIST" << std::endl;
// 	int i;
// 	int j;
// 	for(i = 1; i <= Database::noVars; i++) {
// 		if(watchUsed[i] != 0) {
// 			std::cout << "Watching " << i << ": " << watchUsed[i] << " clauses" << std::endl;
// 			for(j = 0; j < watchUsed[i]; j++) {
// 				Database::printClause(watchList[i][j]);
// 				std::cout << std::endl;
// 			}
// 		}
// 		if(watchUsed[-i] != 0) {
// 			std::cout << "Watching " << -i << ": " << watchUsed[i] << " clauses" << std::endl;
// 			for(j = 0; j < watchUsed[-i]; j++) {
// 				Database::printClause(watchList[-i][j]);
// 				std::cout << std::endl;
// 			}
// 		}
// 	}
// 	std::cout << std::endl << "UNIT LIST" << std::endl;
// 	for(i = 0; i < unitsUsed; i++) {
// 		Database::printClause(units[i]);
// 		std::cout << std::endl;
// 	}
// }

}
