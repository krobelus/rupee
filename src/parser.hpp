#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "structs.hpp"

namespace Parser {
	const boost::regex deletionRegex("^d (.*)");
	const boost::regex headerRegex("p (.*)");
	const boost::regex numVarsRegex("(\\d+)(.*)");
	const boost::regex literalRegex("(-?\\d+)(.*)?");

	// extern int literal;
	// extern int pivot;
	// extern int varliteral;
	// extern bool endclause;
	// extern long offset;
	// extern unsigned int hashValue;
	// extern std::string line;
	// extern boost::smatch result;

	void initialize(parser& p);
	bool openFile(parser& p, bool f);
	bool readHeader(parser& p);
	bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r);
	bool parseClause(parser& p, clause& c, std::string& s, bool& taut);
	bool processClause(parser& p, clause& c, hashtable& h, database& d, proof& r);
	int stringToLiteral(int literal);
}

#endif
