#ifndef __STRUCTS_H_INCLUDED__
#define __STRUCTS_H_INCLUDED__

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>

struct proof {
    long* proofarray;
    int* proofpivots;
    int proofmax;
    int proofused;
    int noPremises;
};

struct proofiterator {
    int position;
    long offset;
    int* pointer;
    int pivot;
    bool kind;
};

struct clause {
	int* clausearray;
	int clausemax;
	int clauseused;
    bool* clauselits;
	bool kind;
};

struct database {
    int* databasearray;
    int databasemax;
    int databaseused;
};

struct hashtable {
    long** rowarray;
    int* rowused;
    int* rowmax;
};

struct parser {
	bool file;
	std::ifstream input;
	int maxVariable;
};

struct stack {
    int* array;
    int* current;
    int* used;
    bool conflict;
};

struct savestack {
    int* current;
    int* used;
    bool conflict;
};

struct assignment {
    bool* satisfied;
    int** trailpositions;
    long* reasons;
};

struct trail {
    stack Stack;
    savestack SaveStack;
    assignment Assignment;
};

struct clauselist{
    long* array;
    int used;
    int max;
};

struct watchlist{
    struct clauselist* longlist;
    struct clauselist* unitlist;
    struct clauselist conflictlist;
};

namespace Objects {
	extern clause Clause;
	extern parser Parser;
	extern database Database;
	extern hashtable HashTable;
	extern proof Proof;
	extern trail Trail;
	extern watchlist WatchList;
	extern proofiterator ProofIterator;
}


#endif
