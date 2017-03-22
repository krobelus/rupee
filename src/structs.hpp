#ifndef __STRUCTS_H_INCLUDED__
#define __STRUCTS_H_INCLUDED__

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>

struct proof {
    long* array;
    int* pivots;
    bool* kinds;
    int max;
    int used;
    int noPremises;
};

struct clause {
	int* clausearray;
	int clausemax;
	int clauseused;
    bool* clauselits;
    int allocatedlits;
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
    int** positions;
    long* reasons;
};

struct watchlist {
    long** array;
    int* used;
    int* max;
};

struct revision {
    int* array;
    int* used;
    bool* lits;
};

struct latency {
    long* array;
    int used;
    int max;
    int pivot;
    int* resolvent;
    int* original;
    int* added;
    bool* lits;
};

struct chain {
    int* array;
    int* used;
    bool* lits;
};

struct checker {
    int position;
    long offset;
    int* pointer;
    int pivot;
    bool kind;
    struct model stack;
    struct watchlist watch;
    struct revision cone;
    struct latency rat;
    struct chain tvr;
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
}


#endif
