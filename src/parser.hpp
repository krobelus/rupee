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

	extern int* buffer_;		//Parse buffer
	extern int buffersize_;		//Maximum size of buffer_
	extern int length_;			//Current size of buffer_
	extern int file_;			//Premise or proof file being parsed
	extern int kind_;			//Introduction or deletion instruction parsed
	extern int literal_;		//Parsed literal
	extern int pivot_;			//Pivot of parsed clause
	extern int varliteral_;		//Absolute value of literal_
	extern bool endclause_;		//Flag for having parsed literal 0.
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
