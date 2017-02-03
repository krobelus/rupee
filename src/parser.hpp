#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>

namespace Parser {
	const boost::regex deletionRegex("^d (.*)");
	const boost::regex headerRegex("p (.*)");
	const boost::regex numVarsRegex("(\\d+)(.*)");
	const boost::regex literalRegex("(-?\\d+)(.*)?");

	extern int* buffer_;
	extern int buffersize_;
	extern int length_;
	extern int phase_;
	extern int file_;
	extern int deletion_;
	extern int literal_;
	extern long offset_;
	extern unsigned int hashValue_;
	extern std::string line_;
	extern std::ifstream input_;
	extern boost::smatch result_;

	void skipToHeader();
	void findMaximumVariable();

	void initialize();

	void readClauses();
	bool parseClause();
	void processClause();

	void deallocate();

	void parse();
}

#endif
