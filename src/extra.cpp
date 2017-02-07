#include <stdlib.h>
#include <iostream>
#include <string>

#include "assignment.hpp"
#include "database.hpp"
#include "extra.hpp"
#include "chain.hpp"
#include "reprocess.hpp"
#include "watchlist.hpp"

int Tools::commentLevel = 0;



namespace Parameters {

std::string pathPremise = "instance.cnf";
std::string pathProof = "proof.drat";
bool verbosity = true;
int clauseBufferSize = 1000;
int databaseSize = 1000000;
int hashDepth = 10;

void setPremise(std::string path) {
    pathPremise = path;
    Blablabla::log("Premise: " + path);
}

void setProof(std::string path) {
    pathProof = path;
    Blablabla::log("Premise: " + path);
}

void setVerbosity(bool value) {
    verbosity = value;
}

void setBufferSize(int value) {
    bufferSize = value;
}

void setDatabaseSize(int value) {
    databaseSize = value;
}

void setHashDepth(int value) {
    hashDepth = value;
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
        std::cout << ans << std::endl;
    }
}

}



int Tools::compare(const void *a, const void *b){
  return (*(int*)a - *(int*)b);
}

void Tools::sortClause(int* cla, int length) {
	qsort(cla, length, sizeof(int), Tools::compare);
}

int Tools::leap(int x) {
	return (x * 3) >> 1;
}

void Tools::comment(std::string str) {
    std::string ans = "";
    for(int i = 0; i < Tools::commentLevel; ++i) {
        ans = ans + ":\t";
    }
    ans = ans + str;
    std::cout << ans << std::endl;
}
void Tools::increaseCommentLevel() {
    Tools::commentLevel++;
}

void Tools::decreaseCommentLevel() {
    Tools::commentLevel--;
}

std::string Tools::offsetToString(long offset) {
    return Tools::pointerToString(Database::getPointer(offset));
}

std::string Tools::pointerToString(int* ptr) {
    std::string ans = "";
    while(*ptr != 0) {
        ans = ans + std::to_string(*ptr) + " ";
        ++ptr;
    }
    ans = ans + "0";
    return ans;
}

std::string Tools::processingStackToString() {
    std::string ans = "";
    int* it;
    for(it = Assignment::stack; it < Assignment::stackUsed; ++it) {
        if(it != Assignment::stackCurrent) {
            ans = ans + std::to_string(*it) + " ";
        } else {
            ans = ans + "[" + std::to_string(*it) + "] ";
        }
    }
    if(it == Assignment::stackCurrent) {
        ans = ans + "[ ]";
    }
    return ans;
}

void Tools::resolutionChain() {
    Tools::increaseCommentLevel();
    for(long* it = Chain::chain; it < Chain::chainUsed; ++it) {
        Tools::comment(Tools::offsetToString(*it));
    }
    Tools::decreaseCommentLevel();
}

void Tools::reprocessingStack() {
    std::string ans = "";
    for(int* it = Reprocess::reprocessStack; it < Reprocess::reprocessUsed; ++it) {
        ans = ans + std::to_string(*it) + " ";
    }
    Tools::comment("Reprocessing stack:   " + ans);
    Tools::comment("Stack state:   " + Tools::processingStackToString());
}

void Tools::watchList(int literal) {
    Tools::increaseCommentLevel();
    for(long* it = WatchList::watchList[literal]; *it != 0; ++it) {
        Tools::comment(Tools::offsetToString(*it));
    }
    Tools::decreaseCommentLevel();
}
