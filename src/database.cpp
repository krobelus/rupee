#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"


namespace Database {

//-------------------------------
// Memory management
//-------------------------------

bool allocate(database& d) {
	#ifdef VERBOSE
	Blablabla::log("Allocating clause database");
	#endif
	d.max = Parameters::databaseSize;
	d.array = (int*) malloc (d.max * sizeof(int));
	d.used = 0;
	d.idCount = 0;
	if(d.array == NULL) {
		#ifdef VERBOSE
		Blablabla::log("Error at clause database allocation");
		#endif
		Blablabla::comment("Memory management error");
		return false;
	} else {
		return true;
	}
}

bool reallocate(database& d) {
	#ifdef VERBOSE
    Blablabla::log("Reallocating clause database");
	#endif
    d.max *= 2;
    d.array = (int*) realloc (d.array, d.max * sizeof(int));
	if(d.array == NULL) {
		#ifdef VERBOSE
		Blablabla::log("Error at clause database reallocation");
		#endif
		Blablabla::comment("Memory management error");
		return false;
	} else {
		return true;
	}
}

void deallocate(database& d) {
	#ifdef VERBOSE
	Blablabla::log("Deallocating clause database");
	#endif
	free(d.array);
}

//-------------------------------
// Pointer-offset conversion
//-------------------------------

long getOffset(database& d, int* ptr) {
	return (long) (ptr - d.array);
}

int* getPointer(database& d, long offset) {
	return (int*) (d.array + offset);
}

//-------------------------------
// Flag / Counter / Id manipulation
//-------------------------------

int copies;
int* cppointer;

int getId(int* ptr) {
	return ptr[Constants::IdCellDatabase];
}

bool isFlag(int* ptr, int bit, bool value) {
	return ((ptr[Constants::FlagsCellDatabase] >> bit) % 2) == value;
}

void setFlag(int* ptr, int bit, bool value) {
	if(value) {
		ptr[Constants::FlagsCellDatabase] |=  (1 << bit);
	} else {
		ptr[Constants::FlagsCellDatabase] &= ~(1 << bit);
	}
}

void setClauseActive(int* ptr) {
	setFlag(ptr, Constants::ActivityBit, Constants::ActiveFlag);
}

void setClauseInactive(int* ptr) {
	setFlag(ptr, Constants::ActivityBit, Constants::InactiveFlag);
}

void setCopies(int* ptr, int cps) {
	ptr[Constants::FlagsCellDatabase] = (ptr[Constants::FlagsCellDatabase] & Constants::CopiesMask) | (cps << Constants::CopiesShift);
}

void addCopy(database& d, long offset) {
	cppointer = getPointer(d, offset);
	copies = (cppointer[Constants::FlagsCellDatabase] >> Constants::CopiesShift);
	setCopies(cppointer, copies + 1);
}

bool removeCopy(database& d, long offset) {
	cppointer = getPointer(d, offset);
	copies = (cppointer[Constants::FlagsCellDatabase] >> Constants::CopiesShift);
	setCopies(cppointer, copies - 1);
	return copies == 1;
}

//-------------------------------
// Clause insertion/removal
//-------------------------------

unsigned int hash;
int it;
int* pointer;
int pos;

bool insertBufferClause(database& d, clause& c, hashtable& h, bool file, long& offset) {
	Clause::sortClause(c);
    hash = HashTable::getHash(c.array);
	if(file == Constants::FilePremise) {
		++d.idCount;
	}
	if(c.taut) {
		#ifdef VERBOSE
		Blablabla::log("Trying to introduce a tautological clause; ignoring instruction");
		#endif
		offset = Constants::NoOffset;
		return true;
	} else if(HashTable::match(h, d, c, hash, offset, pos)) {
		#ifdef VERBOSE
		Blablabla::log("Introducing an already present clause; increasing counter");
		#endif
		addCopy(d, offset);
		offset = Constants::NoOffset;
		return true;
	} else {
		#ifdef VERBOSE
		Blablabla::log("Introducing a missing clause in database");
		#endif
		if(d.used + c.used + Constants::ExtraCellsDatabase >= d.max) {
			if(!reallocate(d)) { return false; }
		}
		pointer = d.array + d.used + Constants::ExtraCellsDatabase;
		if(file == Constants::FilePremise) {
			pointer[Constants::IdCellDatabase] = d.idCount;
		}
		for(it = 0; it <= c.used; ++it) {
			pointer[it] = c.array[it];
		}
		d.used += c.used + Constants::ExtraCellsDatabase + 1;
		setFlag(pointer, Constants::ActivityBit, Constants::InactiveFlag);
		setFlag(pointer, Constants::OriginalityBit, file);
		setFlag(pointer, Constants::PersistencyBit, Constants::PersistentFlag);
		setFlag(pointer, Constants::VerificationBit, Constants::SkipFlag);
		setFlag(pointer, Constants::PseudounitBit, Constants::PassiveFlag);
		setFlag(pointer, Constants::RecheckBit, Constants::InactiveFlag);
		setFlag(pointer, Constants::TrimmingBit, Constants::SkipFlag);
		setCopies(pointer, 1);
		offset = getOffset(d, pointer);
		if(!HashTable::insertOffset(h, hash, offset)) { return false; }
		return true;
	}
}

void removeBufferClause(database& d, clause& c, hashtable& h, long& offset) {
	Clause::sortClause(c);
    hash = HashTable::getHash(c.array);
	if(c.taut) {
		#ifdef VERBOSE
		Blablabla::log("Trying to delete a tautological clause; ignoring instruction");
		#endif
		offset = Constants::NoOffset;
	} else if(HashTable::match(h, d, c, hash, offset, pos)) {
		if(removeCopy(d, offset)) {
			#ifdef VERBOSE
			Blablabla::log("Removing a single-copy clause");
			#endif
			pointer = getPointer(d, offset);
			setFlag(pointer, Constants::PersistencyBit, Constants::TemporalFlag);
			HashTable::removeOffset(h, hash, pos);
		} else {
			#ifdef VERBOSE
			Blablabla::log("Removing a multicopy clause");
			#endif
			offset = Constants::NoOffset;
		}
	} else {
		#ifdef VERBOSE
		Blablabla::log("Trying to delete a missing clause; ignoring instruction");
		#endif
		offset = Constants::NoOffset;
	}
}

bool deriveContradiction(database& d, long& offset) {
	if(d.used + Constants::ExtraCellsDatabase >= d.max) {
		if(!reallocate(d)) { return false; }
	}
	pointer = d.array + d.used + Constants::ExtraCellsDatabase;
	pointer[0] = 0;
	d.used += Constants::ExtraCellsDatabase + 1;
	setFlag(pointer, Constants::ActivityBit, Constants::ActiveFlag);
	setFlag(pointer, Constants::OriginalityBit, Constants::DerivedFlag);
	setFlag(pointer, Constants::PersistencyBit, Constants::PersistentFlag);
	setFlag(pointer, Constants::VerificationBit, Constants::ScheduledFlag);
	setFlag(pointer, Constants::PseudounitBit, Constants::PassiveFlag);
	setFlag(pointer, Constants::RecheckBit, Constants::InactiveFlag);
	setFlag(pointer, Constants::TrimmingBit, Constants::ScheduledFlag);
	setCopies(pointer, 1);
	offset = getOffset(d, pointer);
	return true;
}

//-------------------------------
// Clause tests
//-------------------------------

bool isEmpty(int* ptr) {
	return *ptr == Constants::EndOfList;
}

bool nextNonFalsified(int*& ptr, int& lit, model& m) {
	while((lit = *ptr) != Constants::EndOfList) {
		if(Model::isFalsified(m, lit)) {
			++ptr;
		} else {
			return true;
		}
	}
	return false;
}

bool containsLiteral(int* ptr, int literal) {
	while(*ptr != Constants::EndOfList) {
		if(*(ptr++) == literal) {
			return true;
		}
	}
	return false;
}

//-------------------------------
// Database tests
//-------------------------------

int* dit;
int dlit;
bool dsat;
bool dact;
int dunk;

bool recheckActivity(database& d) {
	dit = d.array + Constants::ExtraCellsDatabase;
	while(dit - d.array < d.used) {
		if(((dit[Constants::FlagsCellDatabase] >> Constants::ActivityBit) % 2) !=
				((dit[Constants::FlagsCellDatabase] >> Constants::RecheckBit) % 2)) {
			return false;
		}
		while(*dit != Constants::EndOfList) { ++dit; }
		dit += 3;
	}
	return true;
}

bool checkUpModel(database& d, bool* lits) {
	#ifdef VERBOSE
	Blablabla::logDatabase(d);
	Blablabla::log("Checking model against formula");
	Blablabla::increase();
	#endif
	dit = d.array + Constants::ExtraCellsDatabase;
	dact = Database::isFlag(dit, Constants::ActivityBit, Constants::ActiveFlag);
	while(dit - d.array < d.used) {
		#ifdef VERBOSE
		Blablabla::log("Clause " + Blablabla::clauseToString(dit));
		#endif
		dsat = false;
		dunk = 0;
		while((dlit = *(dit++)) != Constants::EndOfList) {
			if(lits[dlit]) {
				dsat = true;
			} else if(!lits[-dlit]) {
				++dunk;
			}
		}
		if(!dsat && dunk <= 1 && dact) {
			#ifdef VERBOSE
			Blablabla::log("Unstable UP-model");
			Blablabla::decrease();
			#endif
			return false;
		} else {
			dit += 2;
			dact = Database::isFlag(dit, Constants::ActivityBit, Constants::ActiveFlag);
		}
	}
	#ifdef VERBOSE
	Blablabla::decrease();
	#endif
	return true;
}

}
