#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "hashtable.hpp"
#include "assignment.hpp"
#include "solver.hpp"
#include "chain.hpp"
#include "reprocess.hpp"
#include "resolvent.hpp"
#include "watchlist.hpp"
#include "parser.hpp"

namespace Parser {

struct buffer {
	int* bufferarray;
	int buffermax;
	int bufferused;
	int kind;
} Buffer;

struct info {
	int file;
	std::ifstream input;
	int maxVariable;
} Info;

int literal_;
int pivot_;
int varliteral_;
bool endclause_;
long offset_;
unsigned int hashValue_;
std::string line_;
boost::smatch result_;

int compare(const void *a, const void *b){
  return (*(int*)a - *(int*)b);
}

bool allocate(buffer& b) {
	Blablabla::log("Allocating parse buffer.");
	b.buffermax = Parameters::bufferSize;
	b.bufferused = 0;
	b.bufferarray = (int*) malloc(b.buffermax * sizeof(int));
	if(b.bufferarray == NULL) {
		Blablabla::log("Error at parse buffer allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

bool reallocate(buffer& b) {
	Blablabla::log("Reallocating parse buffer.");
	b.buffermax *= 2;
	(int*) realloc (b.bufferarray, b.buffermax * sizeof(int));
	if(b.bufferarray == NULL) {
		Blablabla::log("Error at parse buffer reallocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void deallocate(buffer& b) {
	Blablabla::log("Deallocating parse buffer.");
	free(b.bufferarray);
}

void reset(buffer& b)  {
	b.bufferused = 0;
}

void addLiteral(buffer& b, int literal) {
	if(b.bufferused >= b.buffermax) {
		reallocate();
	}
	b.bufferarray[b.bufferused++] = literal;
}

void closeClause(buffer& b) {
	if(b.bufferused >= b.buffermax) {
		reallocate();
	}
	b.bufferarray[b.bufferused] = 0;
}

void sortClause(buffer& b) {
	qsort(b.bufferarray, b.bufferused, sizeof(int), compare)
}

void initialize(info& i) {
	i.maxVariable = 0;
}

void finish(info& i) {
	Database::noVariables = i.maxVariable;
}

bool openFile(info& i, int file) {
	i.file = file;
	if(file == Constants::KindOriginal) {
		i.input.open(Parameters::pathPremise);
	} else {
		i.input.open(Parameters::pathProof);
	}
	if(i.input.fail()) {
		Blablabla::log("File was not found.");
		Blablabla::comment("Input error.");
		return false;
	} else {
		return true;
	}
}

bool readHeader(info& i) {
	while(i.input.good()) {
		getline(i.input, line_);
		if(boost::regex_search(line_, result_, headerRegex)) {
			Blablabla::log("Header found.")
			return true;
		}
	}
	Blablabla::log("No header was found in CNF instance.");
	Blablabla::comment("Input error.");
	return false;
}

bool readClauses(info& i, buffer& b) {
	while(i.input.good()) {
		getline(i.input, line_);
		if(!line_.empty()) {
			if(parseClause(i, b, line_)) {
				processClause(i,b);
			} else {
				Blablabla::log("Could not parse file.");
				Blablabla::comment("Input error.");
				return false;
			}
		}
	}
	Blablabla::log("Parsed file.");
	i.input.close();
	return true;
}

bool parseClause(info& i, buffer& b, std::string& line) {
	reset(b);
	if(boost::regex_search(line, result_, deletionRegex)) {
		b.kind = Constants::KindDeletion;
	} else {
		b.kind = Constants::KindIntroduction;
	}
	while(boost::regex_search(line, result_, literalRegex)) {
		literal_ = boost::lexical_cast<int>(result_[1]);
		if(literal_ == 0) {
			closeClause(b);
			return true;
		} else {
			varliteral_ = abs(literal_);
			if(i.maxVariable < varliteral_) {
				i.maxVariable = varliteral_;
			}
			addLiteral(b, literal_);
			line = result_[2];
		}
	}
	return false;
}

void processClause(info& i, buffer& b) {
	pivot_ = b.bufferarray[0];
	sortClause(b);
	hashValue_ = HashTable::getHash(b);
	if(b.kind == Constants::KindIntroduction) {
		offset_ = Database::addClause(b, i.file | Constants::KindSkip | Constants::KindPermanent | Constants::KindActive);
		HashTable::addClause(hashValue_, offset_);
	} else {
		if((offset_ = HashTable::matchAndRemoveClause(b, hashValue_) < 0) {
			Blablabla::log("Attempted to remove a missing clause.");
			Blablabla::comment("Ignoring incorrect deletion instruction.")
		}
		Database::deactivateClause(offset_);
		Database::temporalClause(offset_);
	}
	if(file_ == Constants::KindDerived) {
		Proof::storeInstruction(offset_,pivot_,b.kind);
	}
}




//
//
//
//
//
//
//
// void endParsing() {
// 	Blablabla::log("Deallocating parse buffer.");
// 	free(b.buffer);
// 	Database::noVars = maxVariable_;
// 	Database::noPremises = clauseCounter_;
// }
//
// void resetBuffer()  {
// 	length_ = 0;
// }
//
// void addLiteralToBuffer(int literal) {
// 	varliteral_ = abs(literal);
// 	if(varliteral_ > maxVariable_) {
// 		maxVariable_ = varliteral_;
// 	}
// 	if(length_ >= buffersize_) {
// 		reallocate();
// 	}
// 	buffer_[length_++] = literal;
// }
//
// void closeBuffer() {
// 	if(length_ >= buffersize_) {
// 		reallocate();
// 	}
// 	buffer_[length_] = 0;
// }
//
// bool openFile(int file) {
// 	file_ = file;
// 	if(file_ == Constants::KindOriginal) {
// 		input_.open(Parameters::pathPremise);
// 		file_ = Constants::KindOriginal;
// 	} else {
// 		input_.open(Parameters::pathProof);
// 		file_ = Constants::KindDerived;
// 	}
// 	if(input_.fail()) {
// 		Blablabla::log("File was not found.");
// 		Blablabla::comment("Input error.");
// 		return false;
// 	} else {
// 		return true;
// 	}
// }
//
// bool readHeader() {
// 	while(input_.good()) {
// 		getline(input_, line_);
// 		if(boost::regex_search(line_, result_, headerRegex)) {
// 			Blablabla::log("Header found.")
// 			return true;
// 		}
// 	}
// 	Blablabla::log("No header was found in CNF instance.");
// 	Blablabla::comment("Input error.");
// 	return false;
// }
//
// bool readClauses() {
// 	while(input_.good()) {
// 		getline(input_, line_);
// 		if(!line_.empty() && parseClause(line_)) {
// 			processClause(buffer_, length_, kind_);
// 		} else {
// 			Blablabla::log("Could not parse file.");
// 			Blablabla::comment("Input error.");
// 			return false;
// 		}
// 	}
// 	Blablabla::log("Parsed file.");
// 	input_.close();
// 	return true;
// }
//
// bool parseClause(std::string& line) {
// 	length_ = 0;
// 	endclause_ = false;
// 	if(boost::regex_search(line, result_, deletionRegex)) {
// 		kind_ = Constants::KindDeletion;
// 	} else {
// 		kind_ = Constants::KindIntroduction;
// 	}
// 	while(!endclause_ && boost::regex_search(line, result_, literalRegex)) {
// 		literal_ = boost::lexical_cast<int>(result_[1]);
// 		if(literal_ == 0) {
// 			closeBuffer();
// 			endclause_ == true;
// 		} else {
// 			addLiteralToBuffer(literal_);
// 			line = result_[2];
// 		}
// 	}
// 	return endclause;
// }
//
// void processClause(int* buffer, int length, bool kind) {
// 	pivot_ = buffer[0];
// 	sortClause(buffer, length);
// 	hashValue_ = HashTable::getHash(buffer);
//
// }
//
//
// void processClause() {
// 	literal_ = buffer_[0];
// 	sortClause();		// WRITE.
// 	Tools::sortClause(buffer_, length_);				/// WARNING I think it's not good for performance to sort it before choosing the watched literals...
// 	hashValue_ = HashTable::getHash(buffer_);
// 	if(kind_ == Constants::KindIntroduction) {
// 		offset_ = Database::addClause(buffer_, length_, literal_, file_ | Constants::KindSkip | Constants::KindTemporal);
// 		HashTable::addClause(hashValue_, offset_);
// 		Database::activateClause(offset_);
// 		if(file_ == Constants::KindOriginal) {
// 			Database::setFlag(offset_, Constants::PersistencyFlagDatabase, Constants::KindPermanent);
// 		}
// 	} else {
// 		offset_ = HashTable::matchAndRemoveClause(buffer_,length_, hashValue_);
// 		if(offset_ < 0) {
// 			std::cout << "WARNING: proof deletes a missing clause." << std::endl;
// 			offset_ = Database::addClause(buffer_, length_, literal_, kind_ | file_ | Constants::KindSkip | Constants::KindTemporal);
// 		}
// 		Database::deactivateClause(offset_);
// 		Database::setFlag(offset_, Constants::PersistencyFlagDatabase, Constants::KindTemporal);
// 	}
// 	if(file_ == Constants::KindDerived) {
// 		Proof::storeInstruction(offset_, kind_);
// 	}
// 	if(literal_ == 0) {
// 		if(file_ == Constants::KindOriginal) {
// 			Solver::setStatus(Constants::PremiseContradiction);
// 		} else {
// 			Solver::setStatus(Constants::ProofContradiction);
// 			Database::setFlag(offset_, Constants::VerificationFlagDatabase, Constants::KindVerify);
// 		}
// 	}
// }
//
//
//
//
//
//
//
// void parse() {
// 	std::cout << "Parsing files." << std::endl;
// 	findMaximumVariable();
// 	initialize();
// 	readClauses();
// 	deallocate();
// }
//
// // void findMaximumVariable() {
// // 	Database::noVars = 0;
// // 	phase_ = Constants::ParsingPhasePreprocessPremises;
// // 	input_.open(Parameters::pathPremise);
// // 	while(phase_ < Constants::ParsingPhaseInitialize) {
// // 		if(input_.is_open()) {
// // 			if(phase_ == Constants::ParsingPhasePreprocessPremises) {
// // 				skipToHeader();
// // 			}
// // 			while(input_.good()) {
// // 				getline(input_, line_);
// // 				while(boost::regex_search(line_, result_, literalRegex)) {
// // 					if(Database::noVars < abs(boost::lexical_cast<int>(result_[1]))) {
// // 						Database::noVars = abs(boost::lexical_cast<int>(result_[1]));
// // 					}
// // 					line_ = result_[2];
// // 				}
// // 			}
// // 		} else {
// // 			if(phase_ == Constants::ParsingPhasePreprocessPremises) {
// // 				std::cout << "ERROR: Couldn't open premise file." << std::endl;
// // 			} else {
// // 				std::cout << "ERROR: Couldn't open proof file." << std::endl;
// // 			}
// // 		}
// // 		input_.close();
// // 		if(phase_ == Constants::ParsingPhasePreprocessPremises) {
// // 			input_.open(Parameters::pathProof);
// // 		}
// // 		phase_++;
// // 	}
// // 	std::cout << Database::noVars << " variables detected." << std::endl;
// // }
//
// void skipToHeader() {
// 	while(input_.good()) {
// 		getline(input_, line_);
// 		if(boost::regex_search(line_, result_, headerRegex)) {
// 			return;
// 		}
// 	}
// 	if(phase_ == Constants::ParsingPhasePreprocessPremises) {
// 		std::cout << "ERROR: No header was found in premise file" << std::endl;
// 	}
// }
//
// void initialize() {
// 	std::cout << "Allocating data structures." << std::endl;
// 	buffersize_ = Database::noVars * 2;
// 	buffer_ = (int*) malloc(buffersize_ * sizeof(int));
// 	Database::allocate();
// 	HashTable::allocate();
// 	Proof::allocate();
// 	Assignment::initialize();
// 	WatchList::initialize();
// 	Chain::initialize();
// 	Reprocess::initialize();
// 	Resolvent::initialize();
// 	phase_++;
// }
//
// void readClauses() {
// 	Solver::proofStatus = Constants::NoContradiction;
// 	while(phase_ < Constants::ParsingDeallocate) {
// 		if(phase_ == Constants::ParsingPhaseReadPremises) {		///Prepares to read clauses in the corresponding file
// 			input_.open(Parameters::pathPremise);
// 			skipToHeader();
// 			file_ = Constants::KindOriginal;
// 		} else {
// 			input_.open(Parameters::pathProof);
// 			file_ = Constants::KindDerived;
// 		}
// 		if(input_.is_open()) {				///If the input file is not open, something went wrong and errors are issued.
// 			while(input_.good()) {			/// For every line in the file after headers and such...
// 				getline(input_, line_);
// 				if(!line_.empty()) {
// 					if(parseClause()) {			///... try to parse it as a clause
// 						processClause();			/// If I am successful, I process it
// 						Tools::decreaseCommentLevel();
// 					} else {						/// Otherwise an error is issued
// 						if(phase_ == Constants::ParsingPhasePreprocessPremises) {
// 							std::cout << "ERROR: Couldn't parse premise file." << std::endl;
// 						} else {
// 							std::cout << "ERROR: Couldn't parse proof file." << std::endl;
// 						}
// 					}
// 				}
// 			}
// 		} else {
// 			if(phase_ == Constants::ParsingPhasePreprocessPremises) {
// 				std::cout << "ERROR: Couldn't open premise file." << std::endl;
// 			} else {
// 				std::cout << "ERROR: Couldn't open proof file." << std::endl;
// 			}
// 		}
// 		input_.close();
// 		phase_++;
// 	}
// 	if(Solver::proofStatus == Constants::ProofContradiction) {
// 		std::cout << "Detected contradiction in proof." << std::endl;
// 	} else if(Solver::proofStatus == Constants::PremiseContradiction) {
// 		std::cout << "Detected contradiction in premises." << std::endl;
// 	} else if(Solver::proofStatus == Constants::NoContradiction) {
// 		std::cout << "WARNING: no contradiction in premises or proof." << std::endl;
// 	}
// }
//
// bool parseClause() {
// 	length_ = 0;
// 	if(boost::regex_search(line_, result_, deletionRegex)) {
// 		kind_ = Constants::KindDeletion;
// 	} else {
// 		kind_ = Constants::KindIntroduction;
// 	}
// 	while(boost::regex_search(line_, result_, literalRegex)) {
// 		literal_ = boost::lexical_cast<int>(result_[1]);
// 		if(literal_ == 0) {
// 			buffer_[length_] = 0;
// 			std::string str = "Read instruction ";
// 			if(kind_ == Constants::KindDeletion) {
// 				str = str + "d ";
// 			}
// 			str = str + Tools::pointerToString(buffer_);
// 			Tools::comment(str);
// 			Tools::increaseCommentLevel();
// 			return true;
// 		} else {
// 			buffer_[length_++] = literal_;
// 			line_ = result_[2];
// 		}
// 	}
// 	return false;
// }
//
//
// void processClause() {
// 	literal_ = buffer_[0];								/// Store the pivot, sort the clause and obtain the hash.
// 	Tools::sortClause(buffer_, length_);				/// WARNING I think it's not good for performance to sort it before choosing the watched literals...
// 	hashValue_ = HashTable::getHash(buffer_);
// 	if(kind_ == Constants::KindIntroduction) {
// 		offset_ = Database::addClause(buffer_, length_, literal_, file_ | Constants::KindSkip | Constants::KindTemporal);
// 		HashTable::addClause(hashValue_, offset_);
// 		Database::activateClause(offset_);
// 		if(file_ == Constants::KindOriginal) {
// 			Database::setFlag(offset_, Constants::PersistencyFlagDatabase, Constants::KindPermanent);
// 		}
// 	} else {
// 		offset_ = HashTable::matchAndRemoveClause(buffer_,length_, hashValue_);
// 		if(offset_ < 0) {
// 			std::cout << "WARNING: proof deletes a missing clause." << std::endl;
// 			offset_ = Database::addClause(buffer_, length_, literal_, kind_ | file_ | Constants::KindSkip | Constants::KindTemporal);
// 		}
// 		Database::deactivateClause(offset_);
// 		Database::setFlag(offset_, Constants::PersistencyFlagDatabase, Constants::KindTemporal);
// 	}
// 	if(file_ == Constants::KindDerived) {
// 		Proof::storeInstruction(offset_, kind_);
// 	}
// 	if(literal_ == 0) {
// 		if(file_ == Constants::KindOriginal) {
// 			Solver::setStatus(Constants::PremiseContradiction);
// 		} else {
// 			Solver::setStatus(Constants::ProofContradiction);
// 			Database::setFlag(offset_, Constants::VerificationFlagDatabase, Constants::KindVerify);
// 		}
// 	}
// }
//
// void deallocate() {
// 	free(buffer_);
// 	HashTable::deallocate();
// }

}
