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

bool Parameters::verbosePropagation = true;
std::string Parameters::pathPremise;
std::string Parameters::pathProof;

void Parameters::setInput(std::string path) {
    Parameters::pathPremise = path + ".cnf";
    std::cout << "Premise: " << Parameters::pathPremise << std::endl;
    Parameters::pathProof = path + ".drat";
    std::cout << "Proof: " << Parameters::pathProof << std::endl;
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
