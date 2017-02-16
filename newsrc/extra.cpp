#include <stdlib.h>
#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"

namespace Parameters {

std::string pathPremise;
std::string pathProof;
int bufferSize = 1000;
int databaseSize = 1000000;
bool verbosity = true;
int hashDepth = 10;
int noVariables;

void setPremise(std::string path) {
    pathPremise = path;
    Blablabla::log("Premise: " + path);
}

void setProof(std::string path) {
    pathProof = path;
    Blablabla::log("Proof: " + path);
}

void setVerbosity(bool value) {
    verbosity = value;
}

void setBufferSize(int value) {
    Blablabla::log("Buffer size: " + std::to_string(value));
    bufferSize = value;
}

void setDatabaseSize(int value) {
    Blablabla::log("Database size: " + std::to_string(value));
    databaseSize = value;
}

void setHashDepth(int value) {
    Blablabla::log("Hash table depth size: " + std::to_string(value));
    hashDepth = value;
}

void importNoVariables(parser& p) {
    noVariables = p.maxVariable;
    Blablabla::log("Number of variables: " + std::to_string(noVariables));
}



}




namespace Blablabla {

int level = 0;

void increase() {
    Blablabla::level++;
}

void decrease() {
    Blablabla::level--;
}

void log(std::string str) {
    if(Parameters::verbosity) {
        for(int i = 0; i < level; ++i) {
            std::cout << ":\t";
        }
        std::cout << str << std::endl;
    }
}

void comment(std::string str) {
    std::cout << str << std::endl;
}

}
