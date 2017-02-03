#ifndef __DATABASE_H_INCLUDED__
#define __DATABASE_H_INCLUDED__

#include <stdlib.h>

namespace Database {
	extern int* db;
	extern int max;
	extern int used;
	extern int noVars;
	extern int noClauses;

	extern int* clause_;
	extern int* ptr_;
	extern int it_;

	void allocate();
	void reallocate();
	void deallocate();

	long addClause(int* cla, int size, int pivot, int flags);

	void setFlag(long offset, int flag, int value);
	bool isFlag(long offset, int flag, int value);
	bool mustVerify(int* clause);

	long getOffset(int* position);
	int* getPointer(long offset);

	void activateClause(long offset);
	void deactivateClause(long offset);

	void print();
	void printField();
	void printClause(long offset);
}

#endif
