#include <iostream>

#include "extra.hpp"
#include "watchlist.hpp"
#include "database.hpp"

namespace Database {

int* db;
int max;
int used;
int noVars;
int noClauses;
int* clause_;
int* ptr_;
int it_;

bool allocate() {
	Blablabla::log("Allocating clause database.");
	max = Parameters::databaseSize;
	db = (int*) malloc (max * sizeof(int));
	used = 0;
	if(db == NULL) {
		Blablabla::log("Error at clause database allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void reallocate() {
	std::cout << "Reallocating clause database." << std::endl;
	max = Tools::leap(max);
	db = (int*) realloc (db, max * sizeof(int));
}

void deallocate() {
	std::cout << "Deallocating clause database." << std::endl;
	free(db);
}

void setNoPremises(int value) {
	noPremises = value;
}
void setNoVariables(int value) {
	noVars = value;
}

long addClause(int* cla, int size, int pivot, int flags) {
	if(used + size + Constants::ExtraCellsDatabase >= max) {
		reallocate();
	}
	clause_ = db + used + Constants::ExtraCellsDatabase - 1;
	clause_[Constants::PivotCellDatabase] = pivot;
	clause_[Constants::FlagsCellDatabase] = flags;
	for(it_ = 0; it_ < size; ++it_) {
		clause_[it_] = cla[it_];
	}
	clause_[size] = 0;
	used += size + Constants::ExtraCellsDatabase;
	return getOffset(clause_);
}

//
// long addClause(int* cla, int size, int pivot, int flags) {
// 	if(used + size + Constants::ExtraCellsDatabase >= max) {
// 		reallocate();
// 	}
// 	clause_ = db + used + Constants::ExtraCellsDatabase - 1;
// 	clause_[Constants::PivotCellDatabase] = pivot;
// 	clause_[Constants::FlagsCellDatabase] = flags;
// 	for(int i = 0; i < size; ++i) {
// 		clause_[i] = cla[i];
// 	}
// 	clause_[size] = 0;
// 	used += size + Constants::ExtraCellsDatabase;
// 	Tools::comment("Inserted clause " + Tools::pointerToString(clause_));
// 	return getOffset(clause_);
// }

void setFlag(long offset, int flag, int value) {
	ptr_ = Database::db + offset + Constants::FlagsCellDatabase;
	if((value >> flag) % 2 == 1) {
		*ptr_ = *ptr_ | (1 << flag);
	} else {
		*ptr_ = *ptr_ & ~(1 << flag);
	}
}

bool isFlag(long offset, int flag, int value) {
	ptr_ = Database::db + offset + Constants::FlagsCellDatabase;
	return (*ptr_ << flag) % 2 == value;
}

bool mustVerify(int* clause) {
	return ((clause[Constants::FlagsCellDatabase] >> 1) % 2) != 0;
}

long getOffset(int* pointer){
	return (long) (pointer - db);
}

int* getPointer(long offset){
	return db + offset;
}

void activateClause(long offset) {
	if(isFlag(offset, Constants::ActivityFlagDatabase, Constants::KindInactive)) {
		Database::setFlag(offset, Constants::ActivityFlagDatabase, Constants::KindActive);
		WatchList::introduceClause(offset);
	}
}

void deactivateClause(long offset) {
	if(isFlag(offset, Constants::ActivityFlagDatabase, Constants::KindActive)) {
		Database::setFlag(offset, Constants::ActivityFlagDatabase, Constants::KindInactive);
		WatchList::removeClause(offset);
	}
}

void printField() {
	Tools::comment("DATABASE FIELD:");
	std::string str = "";
	for(int i = 0; i < Database::used; ++i) {
		str = str + std::to_string(Database::db[i]) + " ";
	}
	Tools::comment(str);
	Tools::comment("---------------------------------------");
	Tools::comment("");
}

void print() {
	bool done;
	int j;
	Tools::comment("DATABASE:");
	Tools::comment("\t@\tactive clause");
	Tools::comment("\t!\tinput clause");
	Tools::comment("\tX\tscheduled for verification");
	for(int i = 2; i < used;) {
		ptr_ = &(db[i]);
		done = false;
		j = 0;
		while(!done) {
			std::cout << ptr_[j] << " ";
			i++;
			if(ptr_[j] == 0) {
				done = true;
				std::cout << "  [" << ptr_[Constants::PivotCellDatabase] << "]  ";
				if((ptr_[Constants::FlagsCellDatabase] >> Constants::ActivityFlagDatabase) % 2 != 0) {
					std::cout << "@";
				}
				if((ptr_[Constants::FlagsCellDatabase] >> Constants::VerificationFlagDatabase) % 2 != 0) {
					std::cout << "X";
				}
				if((ptr_[Constants::FlagsCellDatabase] >> Constants::OriginalityFlagDatabase) % 2 == 0) {
					std::cout << "!";
				}
				std::cout << std::endl;
				i += Constants::ExtraCellsDatabase - 1;
			}
			j++;
		}
	}
	std::cout << std::endl;
}

void printClause(long offset) {
	ptr_ = getPointer(offset);
	int i = 0;
	while(ptr_[i] != 0) {
		std::cout << ptr_[i] << " ";
		i++;
	}
	std::cout << "0";
}

}
