#ifndef __WATCHLIST_H_INCLUDED__
#define __WATCHLIST_H_INCLUDED__

#include <stdlib.h>

namespace WatchList {
	extern long** watchList;
	extern int* watchUsed;
	extern int* watchMax;

	extern long** unitList;
	extern int* unitUsed;
	extern int* unitMax;

	extern long* queueList;
	extern int queueUsed;
	extern int queueMax;

	extern long* conflictList;
	extern int conflictUsed;
	extern int conflictMax;

	extern long offset_;
	extern int* clause_;
	extern int* rowlit_;
	extern int* watchedlit_;
	extern int* replaceWatch_;
	extern int* firstWatch_;
	extern int literal_;
	extern int tempLiteral_;
	extern bool firstwatchfound_;
	extern int watchlistIterator_;
	extern bool checkpropagation_;
	extern bool rowpropagation_;
	extern long* removeIterator_;
	extern int unpLiteral_;
	extern long unpOffset_;
	extern int* unpClause_;
	extern bool unpDone_;
	extern int unpConflictIterator_;

	void initialize();
	void deallocate();

	void addToList(long offset, long* &list, int &used, int &max);
	void removeFromList(long offset, long* &list, int &used);
	void removeInList(int pos, long* &list, int &used);

	void addHardConflict(long offset);
	void addSoftConflict(long offset);
	void addUnit(long offset, int literal);
	void addLargeClause(long offset, int* clause);

	void introduceClause(long offset);
	void removeClause(long offset);

	void unpropagateConflicts();
	bool processUnits();
	bool processWatchHard(int row, int &position);
	bool processWatchSoft(int row, int &position);

	bool reprocessWatch(int row, int &position);

	void printWatches();
	void printConflicts();
	void printQueue();
}

#endif
