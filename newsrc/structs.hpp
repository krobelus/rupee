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

struct model {
    int* start;
    int* forced;
    int* head;
    int* used;
    int* satisfied;
    int* positions;
};

struct watchlist {
    long** array;
    int* used;
    int* max;
}

struct checker {
    int position;
    long offset;
    int* pointer;
    int pivot;
    bool kind;
    struct model stack;
    struct watchlist watch;
};

namespace Objects {
	extern clause Clause;
	extern parser Parser;
	extern database Database;
	extern hashtable HashTable;
	extern proof Proof;
    extern model Model;
    extern watchlist WatchList;
    extern checker Checker;
	// extern trail Trail;
	// extern watchlist WatchList;
	// extern proofiterator ProofIterator;
}


#endif
