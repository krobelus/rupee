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
int parsed;
int number;
int othernumber;

bool openFile(parser& p, bool f) {
    p.file = f;
    if(f == Constants::FilePremise) {
        p.inputfile = std::fopen(Parameters::pathPremise.c_str(), "r");
	} else {
        p.inputfile = std::fopen(Parameters::pathProof.c_str(), "r");
	}
    if(p.inputfile == NULL) {
        #ifdef VERBOSE
		Blablabla::log("File was not found.");
        #endif
		Blablabla::comment("Input error.");
		return false;
    }
    return true;
}

bool readHeader(parser& p) {
    while(true) {
        parsed = fscanf(p.inputfile, "p cnf %i %i \n", &number, &othernumber);
        if(parsed == EOF) {
            #ifdef VERBOSE
        	Blablabla::log("No header was found in CNF instance.");
            #endif
        	Blablabla::comment("Input error.");
        	return false;
        }
        if(parsed == 2) {
            #ifdef VERBOSE
			Blablabla::log("Header found.");
            #endif
			return true;
        }
        parsed = fscanf(p.inputfile, "%*s\n");
    }
}

bool inclause;

bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r) {
    inclause = false;
    while(true) {
        if(!inclause) {
            if((parsed = fscanf(p.inputfile, " d %i ", &number)) == 1) {
                Clause::setInstructionKind(c, Constants::InstructionDeletion);
            } else if((parsed = fscanf(p.inputfile, " %i ", &number)) == 1) {
                Clause::setInstructionKind(c, Constants::InstructionIntroduction);
                if(p.file == Constants::FilePremise) {
                    ++d.idCount;
                }
            } else if(parsed == EOF) {
                #ifdef VERBOSE
                Blablabla::log("Parsed file");
                #endif
                fclose(p.inputfile);
                return true;
            } else {
                #ifdef VERBOSE
                Blablabla::log("Could not parse file");
                #endif
                Blablabla::comment("Input error");
                return false;
            }
            inclause = true;
        } else {
            parsed = fscanf(p.inputfile, " %i ", &number);
            if(parsed == EOF) {
                #ifdef VERBOSE
                Blablabla::log("Could not parse file");
                #endif
                Blablabla::comment("Input error");
                return false;
            }
        }
        if(parsed == 1) {
            if(!Clause::processLiteral(c, number)) { return false; }
            if(number == Constants::EndOfList) {
                if(p.file == Constants::FilePremise) {
                    ++Stats::premiseLength;
                } else {
                    ++Stats::proofLength;
                }
                if(!Clause::processInstruction(c, h, d, r, p.file)) { return false; }
                inclause = false;
                Clause::resetBuffer(c);
            }
        } else {
            #ifdef VERBOSE
            Blablabla::log("Could not parse file");
            #endif
            Blablabla::comment("Input error");
            return false;
        }
    }
}

// bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r) {
//     while(p.input.good()) {
// 		getline(p.input, line);
// 		if(!line.empty()) {
// 			if(Clause::parseInstruction(c, line)) {
//                 if(!Clause::processInstruction(c, h, d, r, p.file)) { return false; }
// 			} else {
//                 #ifdef VERBOSE
// 				Blablabla::log("Could not parse file.");
//                 #endif
// 				Blablabla::comment("Input error.");
// 				return false;
// 			}
//             Clause::resetBuffer(c);
// 		}
// 	}
//     #ifdef VERBOSE
// 	Blablabla::log("Parsed file.");
//     #endif
// 	p.input.close();
// 	return true;
// }

}
