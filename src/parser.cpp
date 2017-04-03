#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "structs.hpp"
#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "hashtable.hpp"
#include "clause.hpp"
#include "parser.hpp"

namespace Parser {

int literal;
int pivot;
int varliteral;
long offset;
unsigned int hash;
std::string line;
boost::smatch result;
std::string remainder;
bool inbounds;
bool tautology;
bool done;

void initialize(parser& p) {
    p.maxVariable = 0;
}

bool openFile(parser& p, bool f) {
    p.file = f;
	if(f == Constants::FilePremise) {
		p.input.open(Parameters::pathPremise);
	} else {
		p.input.open(Parameters::pathProof);
	}
	if(p.input.fail()) {
		Blablabla::log("File was not found.");
		Blablabla::comment("Input error.");
		return false;
	} else {
		return true;
	}
}

bool readHeader(parser& p) {
    while(p.input.good()) {
		getline(p.input, line);
		if(boost::regex_search(line, result, headerRegex)) {
			Blablabla::log("Header found.");
			return true;
		}
	}
	Blablabla::log("No header was found in CNF instance.");
	Blablabla::comment("Input error.");
	return false;
}

bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r) {
    while(p.input.good()) {
		getline(p.input, line);
		if(!line.empty()) {
            tautology = false;
			if(parseClause(p, c, line, tautology)) {
                if(!tautology) {
                    processClause(p, c, h, d, r);
                } else {
                    Blablabla::log("Attempted to introduce a tautological clause.");
                    Blablabla::comment("Ignoring incorrect introduction instruction.");
                }
			} else {
				Blablabla::log("Could not parse file.");
				Blablabla::comment("Input error.");
				return false;
			}
		}
	}
	Blablabla::log("Parsed file.");
	p.input.close();
	return true;
}

bool parseClause(parser& p, clause& c, std::string& s, bool& taut) {
	if(p.file == Constants::FileProof && boost::regex_search(s, result, deletionRegex)) {
		c.kind = Constants::InstructionDeletion;
	} else {
		c.kind = Constants::InstructionIntroduction;
	}
    done = false;
    taut = false;
    Clause::resetBuffer(c);
    remainder = s;
    // std::cout << "starting" << std::endl;
    while(!done) {
        if(!boost::regex_search(s, result, literalRegex)) {
            // std::cout << "fail" << std::endl;
            return false;
        } else {
            literal = stringToLiteral(boost::lexical_cast<int>(result[1]));
            if(literal == 0) {
                // std::cout << "end" << std::endl;
                if(c.clauseused == 1) {
                    // std::cout << "adding special" << std::endl;
                    if(!Clause::addLiteral(c, -Constants::ReservedLiteral, inbounds, taut)) { return false; }
                }
                if(!Clause::closeBuffer(c)) { return false; }
                done = true;
            } else if(!Clause::addLiteral(c, literal, inbounds, taut)) {
                // std::cout << "fail" << std::endl;
                return false;
            } else if(!inbounds) {
                // std::cout << "out of bounds" << std::endl;
                if(!Clause::reallocateFlags(c)) { return false; }
                remainder = s;
            } else if(taut) {
                // std::cout << "taut" << std::endl;
                done = true;
            } else {
                // std::cout << "good "<< literal << std::endl;
                varliteral = abs(literal);
    			if(p.maxVariable < varliteral) {
    				p.maxVariable = varliteral;
    			}
                s = result[2];
            }
        }
    }
    return true;
}


bool processClause(parser& p, clause& c, hashtable& h, database& d, proof& r) {
    pivot = c.clausearray[0];
    Clause::sortClause(c);
    hash = HashTable::getHash(c.clausearray);
    if(c.kind == Constants::InstructionIntroduction) {
        Blablabla::log("Parsed instruction: +++" + Blablabla::clauseToString(c.clausearray));
    } else {
        Blablabla::log("Parsed instruction: ---" + Blablabla::clauseToString(c.clausearray));
    }
    if(!HashTable::match(h, d, c, hash, offset)) {
        if(c.kind == Constants::InstructionIntroduction) {
            if(!Database::addBufferClause(d, c, offset)) { return false; }
            if(!HashTable::insertOffset(h, hash, offset)) { return false;}
            if(p.file == Constants::FileProof) {
                Database::setNewIntroductionFlags(d, offset);
            } else {
                Database::setPremiseFlags(d, offset);
            }
        } else {
            Blablabla::log("Attempted to remove a missing clause.");
            Blablabla::comment("Ignoring incorrect deletion instruction.");
            return true;
        }
    } else {
        if(c.kind == Constants::InstructionIntroduction) {
            Database::setOldIntroductionFlags(d, offset);
        } else {
            Database::setDeletionFlags(d, offset);
        }
    }
    if(p.file == Constants::FileProof) {
        if(!Proof::storeInstruction(r, offset, pivot, c.kind)) { return false; }
    } else {
        if(!Proof::storePremise(r, offset)) { return false; }
    }
    return true;
}

int stringToLiteral(int literal) {
    if(literal < 0) {
        return literal - 1;
    } else if(literal > 0) {
        return literal + 1;
    } else {
        return 0;
    }
}

}
