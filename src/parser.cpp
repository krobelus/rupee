#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "structs.hpp"
#include "extra.hpp"


namespace Parser {

std::string line;
boost::smatch result;

bool openFile(parser& p, bool f) {
    p.file = f;
	if(f == Constants::FilePremise) {
		p.input.open(Parameters::pathPremise);
	} else {
		p.input.open(Parameters::pathProof);
	}
	if(p.input.fail()) {
        #ifdef VERBOSE
		Blablabla::log("File was not found.");
        #endif
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
            #ifdef VERBOSE
			Blablabla::log("Header found.");
            #endif
			return true;
		}
	}
    #ifdef VERBOSE
	Blablabla::log("No header was found in CNF instance.");
    #endif
	Blablabla::comment("Input error.");
	return false;
}

bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r) {
    while(p.input.good()) {
		getline(p.input, line);
		if(!line.empty()) {
			if(Clause::parseInstruction(c, line)) {
                if(!Clause::processInstruction(c, h, d, r, p.file)) { return false; }
			} else {
                #ifdef VERBOSE
				Blablabla::log("Could not parse file.");
                #endif
				Blablabla::comment("Input error.");
				return false;
			}
            Clause::resetBuffer(c);
		}
	}
    #ifdef VERBOSE
	Blablabla::log("Parsed file.");
    #endif
	p.input.close();
	return true;
}

}
