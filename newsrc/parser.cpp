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

void initialize(parser& p) {
    p.maxVariable = 0;
    p.noPremises = 0;
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
			if(parseClause(p, c, line)) {
				processClause(p, c, h, d, r);
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

bool parseClause(parser& p, clause& c, std::string& s) {
    Clause::resetBuffer(c);
	if(p.file == Constants::FileProof && boost::regex_search(s, result, deletionRegex)) {
		c.kind = Constants::InstructionDeletion;
	} else {
		c.kind = Constants::InstructionIntroduction;
	}
	while(boost::regex_search(s, result, literalRegex)) {
		literal = boost::lexical_cast<int>(result[1]);
		if(literal == 0) {
			Clause::closeBuffer(c);
			return true;
		} else {
			varliteral = abs(literal);
			if(p.maxVariable < varliteral) {
				p.maxVariable = varliteral;
			}
			if(!Clause::addLiteral(c, literal)) { return false; }
			s = result[2];
		}
	}
	return false;
}


bool processClause(parser& p, clause& c, hashtable& h, database& d, proof& r){
    pivot = c.clausearray[0];
    Clause::sortClause(c);
    hash = HashTable::getHash(c.clausearray);
    Blablabla::log("Parsed clause: " + Clause::printClause(c));
    if(!HashTable::match(h, d, c, hash, offset)) {
        if(c.kind == Constants::InstructionIntroduction) {
            if(!Database::addBufferClause(d, c, offset)) { return false; }
            if(!HashTable::insertOffset(h, hash, offset)) { return false;}
        } else {
            Blablabla::log("Attempted to remove a missing clause.");
            Blablabla::comment("Ignoring incorrect deletion instruction.");
            return true;
        }
    }
    if(p.file == Constants::FileProof) {
        Proof::storeInstruction(r, offset, pivot, c.kind);
    } else {
        ++(p.noPremises);
    }
    return true;
}

}
