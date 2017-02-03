#ifndef __HASHTABLE_H_INCLUDED__
#define __HASHTABLE_H_INCLUDED__

#include <stdlib.h>

namespace HashTable {
	extern long** table;
	extern int* rowUsed;
	extern int* rowMax;

	void allocate();
	void deallocate();
	void reallocateRow(int i);
	unsigned int getHash(int* cla);
	void removeClause(unsigned int hashValue, int position);
	long matchAndRemoveClause(int* cla, int length, unsigned int hashValue);
	void addClause(unsigned int hashValue, long offset);
}

#endif
