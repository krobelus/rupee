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

bool allocate(database& d) {
	Blablabla::log("Allocating clause database.");
	d.databasemax = Parameters::databaseSize;
	d.databasearray = (int*) malloc (d.databasemax * sizeof(int));
	d.databaseused = 0;
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
    d.databasemax = (d.databasemax * 3) >> 1;
    d.databasearray = (int*) realloc (d.databasearray, d.databasemax * sizeof(int));
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

int classifyClause(int* ptr) {
	if(*ptr == 0) {
		return Constants::ClauseSizeConflict;
	} else if(*(++ptr) == 0) {
		return Constants::ClauseSizeUnit;
	} else {
		return Constants::ClauseSizeLong;
	}
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

void setPremiseFlags(int* ptr) {
	Database::setFlag(ptr, Constants::ActivityBit, Constants::ActiveFlag);
	Database::setFlag(ptr, Constants::OriginalityBit, Constants::OriginalFlag);
	Database::setFlag(ptr, Constants::VerificationBit, Constants::SkipFlag);
	Database::setFlag(ptr, Constants::PersistencyBit, Constants::PersistentFlag);
	Database::setFlag(ptr, Constants::PseudounitBit, Constants::RedundantFlag);
	Database::setFlag(pointer, Constants::RawnessBit, Constants::ProcessedFlag);
}

void setIntroductionFlags(int* ptr) {
	Database::setFlag(ptr, Constants::ActivityBit, Constants::ActiveFlag);
	Database::setFlag(ptr, Constants::OriginalityBit, Constants::DerivedFlag);
	Database::setFlag(ptr, Constants::VerificationBit, Constants::SkipFlag);
	if(Database::isFlag(ptr, Constants::RawnessBit, Constants::RawFlag)) {
		Database::setFlag(ptr, Constants::PersistencyBit, Constants::PersistentFlag);
	}
	Database::setFlag(ptr, Constants::PseudounitBit, Constants::RedundantFlag);
	Database::setFlag(pointer, Constants::RawnessBit, Constants::ProcessedFlag);
}

void setDeletionFlags(int* ptr) {
	Database::setFlag(ptr, Constants::ActivityBit, Constants::InactiveFlag);
	Database::setFlag(ptr, Constants::PersistencyBit, Constants::TemporalFlag);
}

void log(database& d) {
	Blablabla::log("Database:");
	Blablabla::increase();
	Blablabla::log("#\tactive");
	Blablabla::log("X\tscheduled");
	Blablabla::log("@\tpremise");
	Blablabla::log("!\tpersistent");
	Blablabla::log("*\treason clause");
	Blablabla::log("");
	int i = 0;
	bool inclause = false;
	std::string str;
	std::string symbols[5];
	symbols[0] = "#";
	symbols[1] = "X";
	symbols[2] = "@";
	symbols[3] = "!";
	symbols[4] = "*";
	while(i < d.databaseused) {
		if(!inclause) {
			inclause = true;
			str = "";
			for(int j = 0; j <= 4; j++ ) {
				if (((d.databasearray[i] >> j) % 2) != 0) {
					str = str + symbols[j] + " ";
				} else {
					str = str + "  ";
				}
			}
			str = str + "  [ ";
		} else {
			if(d.databasearray[i] != 0) {
				str = str + std::to_string(d.databasearray[i]) + " ";
			} else {
				str = str + "]";
				Blablabla::log(str);
				inclause = false;
			}
		}
		i++;
	}
	str = "";
	for(i = 0; i < d.databaseused; ++i) {
		str = str + std::to_string(d.databasearray[i]);
		if(i % 30 == 29) {
			Blablabla::log(str);
			str = "";
		} else {
			str = str + " ";
		}
	}
	Blablabla::log(str);
	Blablabla::decrease();
}

std::string offsetToString(database& d, long offset) {
	std::string str = "[ ";
	int* pointer = getPointer(d, offset);
	while(*pointer != 0) {
		str = str + std::to_string(*(pointer++)) + " ";
	}
	str = str + "]";
	return str;
}

}
