#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>

struct parser {
	int file;
	std::ifstream input;
	int maxVariable;
};

namespace Parser {
	const boost::regex deletionRegex("^d (.*)");
	const boost::regex headerRegex("p (.*)");
	const boost::regex numVarsRegex("(\\d+)(.*)");
	const boost::regex literalRegex("(-?\\d+)(.*)?");

	extern int literal;
	extern int pivot;
	extern int varliteral;
	extern bool endclause;
	extern long offset;
	extern unsigned int hashValue;
	extern std::string line;
	extern boost::smatch result;

	void initialize(parser& p);
	bool openFile(parser& p, int f);
	bool readHeader(parser& p);
	bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r);
	bool parseClause(parser& p, clause& c, std::string& s);
	bool processClause(parser& p, clause& c, hashtable& h, database& d, proof& r);
}

#endif
