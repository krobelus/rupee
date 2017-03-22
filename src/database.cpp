#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"
#include "clause.hpp"
#include "proof.hpp"
#include "database.hpp"

namespace Database {

int it;
int* pointer;
bool inclause;

bool allocate(database& d) {
	Blablabla::log("Allocating clause database.");
	d.databasemax = Parameters::databaseSize;
	d.databasearray = (int*) malloc (d.databasemax * sizeof(int));
	d.databaseused = 0;
	d.current = d.databasearray + d.databaseused;
	if(d.databasearray == NULL) {
		Blablabla::log("Error at clause database allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

bool reallocate(database& d) {
    Blablabla::log("Reallocating clause database.");
    d.databasemax *= 2;
    d.databasearray = (int*) realloc (d.databasearray, d.databasemax * sizeof(int));
	d.current = d.databasearray + d.databaseused;
	if(d.databasearray == NULL) {
		Blablabla::log("Error at clause database reallocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void deallocate(database& d) {
	Blablabla::log("Deallocating clause database.");
	free(d.databasearray);
}

long getOffset(database& d, int* ptr) {
	return (long) (ptr - d.databasearray);
}

int* getPointer(database& d, long offset) {
	return (int*) (d.databasearray + offset);
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

bool isEmpty(int* ptr) {
	return *ptr == 0;
}

bool addBufferClause(database& d, clause& c, long& offset) {
	if(d.databaseused + c.clauseused + Constants::ExtraCellsDatabase >= d.databasemax) {
		if(!reallocate(d)) { return false; }
	}
	pointer = d.databasearray + d.databaseused + Constants::ExtraCellsDatabase;
	for(it = 0; it <= c.clauseused; ++it) {
		pointer[it] = c.clausearray[it];
	}
	d.databaseused += c.clauseused + Constants::ExtraCellsDatabase + 1;
	offset = getOffset(d, pointer);
	Database::setFlag(pointer, Constants::RawnessBit, Constants::RawFlag);
	return true;
}

void setPremiseFlags(database& d, long offset) {
	pointer = Database::getPointer(d, offset);
	Database::setFlag(pointer, Constants::ActivityBit, Constants::InactiveFlag);
	Database::setFlag(pointer, Constants::VerificationBit, Constants::SkipFlag);
	Database::setFlag(pointer, Constants::OriginalityBit, Constants::OriginalFlag);
	Database::setFlag(pointer, Constants::PersistencyBit, Constants::PersistentFlag);
	Database::setFlag(pointer, Constants::PseudounitBit, Constants::RedundantFlag);
	Database::setFlag(pointer, Constants::ConflictBit, Constants::SatisfiableFlag);
	Database::setFlag(pointer, Constants::RawActivity, Constants::ActiveFlag);
}

void setNewIntroductionFlags(database& d, long offset) {
	pointer = Database::getPointer(d, offset);
	Database::setFlag(pointer, Constants::ActivityBit, Constants::InactiveFlag);
	Database::setFlag(pointer, Constants::VerificationBit, Constants::SkipFlag);
	Database::setFlag(pointer, Constants::OriginalityBit, Constants::DerivedFlag);
	Database::setFlag(pointer, Constants::PersistencyBit, Constants::PersistentFlag);
	Database::setFlag(pointer, Constants::PseudounitBit, Constants::RedundantFlag);
	Database::setFlag(pointer, Constants::ConflictBit, Constants::SatisfiableFlag);
	Database::setFlag(pointer, Constants::RawActivity, Constants::ActiveFlag);
}

void setOldIntroductionFlags(database& d, long offset) {
	pointer = Database::getPointer(d, offset);
	Database::setFlag(pointer, Constants::OriginalityBit, Constants::DerivedFlag);
	Database::setFlag(pointer, Constants::RawActivity, Constants::ActiveFlag);
}

void setDeletionFlags(database& d, long offset) {
	pointer = Database::getPointer(d, offset);
	Database::setFlag(pointer, Constants::PersistencyBit, Constants::TemporalFlag);
	Database::setFlag(pointer, Constants::RawActivity, Constants::InactiveFlag);
}

void setClauseActive(database& d, int* ptr) {
	Database::setFlag(pointer, Constants::ActivityBit, Constants::ActiveFlag);
}

void setClauseInactive(database& d, int* ptr) {
	Database::setFlag(pointer, Constants::ActivityBit, Constants::InactiveFlag);
}

bool containsLiteral(database& d, int* ptr, int literal) {
	while(*ptr != Constants::EndOfClause) {
		if(*ptr == literal) {
			return true;
		}
	}
	return false;
}

}
