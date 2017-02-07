#include <iostream>

#include "extra.hpp"
#include "database.hpp"
#include "hashtable.hpp"

namespace HashTable {

long** table;
int* rowUsed;
int* rowMax;

bool allocate() {
	bool error = false;
	Blablabla::log("Allocating hash table.");
	table = (long**) malloc (Parameters::databaseSize * sizeof (long*));
	rowUsed = (int*) malloc (Parameters::databaseSize * sizeof (int));
	rowMax = (int*) malloc (Parameters::databaseSize * sizeof (int));
	if(table == NULL || rowUsed == NULL || rowMax == NULL) {
		error = true;
	}
	for(int i = 0; i < Parameters::databaseSize && !error; ++i) {
		table[i] = (long*) malloc (Parameters::hashDepth * sizeof (long));
		rowMax[i] = Parameters::hashDepth;
		rowUsed[i] = 0;
		if(table[i] == NULL) {
			error = true;
		}
	}
	if(error) {
		Blablabla::log("Error at hash table allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void deallocate() {
	std::cout << "Deallocating hash table." << std::endl;
	for(int i = 0; i < Constants::HashCycle; ++i) {
		free(table[i]);
	}
	free(table);
	free(rowUsed);
	free(rowMax);
}

void reallocateRow(int i) {
	std::cout << "Reallocating hash table." << std::endl;
	rowMax[i] = Tools::leap(rowMax[i]);
	table[i] = (long*) realloc (table[i], rowMax[i] * sizeof (long));
}

unsigned int getHash(int* cla) {
	unsigned int sum = 0;
	unsigned int prod = 1;
	unsigned int xoor = 0;
	while(*cla) {
		prod *= *cla;
		sum += *cla;
		xoor ^= *cla;
		cla++;
	}
	return (((1023 * sum + prod) ^ (31 * xoor))) % Constants::HashCycle;
}

void removeClause(unsigned int hashValue, int position) {
	table[hashValue][position] = table[hashValue][ --rowUsed[hashValue] ];
}

long matchAndRemoveClause(int* clause, int length, unsigned int hashValue) {
	long* clauseList = table[hashValue];
	int listLength = rowUsed[hashValue];
	int* clause0;
	bool matched;
	long offset;
	for(int i = 0; i < listLength; ++i) {				//For every clause with the same hash code
		clause0 = Database::getPointer(clauseList[i]);
		matched = true;
		if((clause0[Constants::FlagsCellDatabase] >> Constants::ActivityFlagDatabase) % 2 != 0) { 	//Check that the clause is active
			for(int j = 0; j <= length && matched; j++) {											//  and all literals are the same
				if(clause0[j] != clause[j]) {
					matched = false;
				}
			}
			if(matched) {								//If so, remove the clause from the hash table and return its offset
				offset = clauseList[i];
				removeClause(hashValue, i);
				return offset;
			}
		}
	}
	return -1;											//If no match is found, then return a negative integer
}

void addClause(unsigned int hashValue, long offset) {//TODO clause must be ordered by now
	if(rowUsed[hashValue] >= rowMax[hashValue]) {
		reallocateRow(hashValue);
	}
	table[hashValue][rowUsed[hashValue]++] = offset;
}

}
