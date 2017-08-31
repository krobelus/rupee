#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

struct lrat {
    int* dbarray;
    int dbmax;
    int dbused;
    int** ptrarray;
    int ptrmax;
    bool* lits;
    int* stack;
    int* stackused;
    int nolits;
};

bool allocate(lrat& l) {
    l.dbmax = 100000;
    l.dbused = 0;
    l.dbarray = (int*) malloc(l.dbmax * sizeof(int));
    return !(l.dbarray == NULL);
}

bool allocatePtr(lrat& l, int n) {
    l.ptrarray = (int**) malloc(n * sizeof(int*));
    l.ptrmax = n;
    if(l.ptrarray == NULL) {
        return false;
    }
    for(int i = 0; i < n; i++) {
        l.ptrarray[i] = NULL;
    }
    return true;
}

bool allocateLits(lrat& l, int n) {
    l.nolits = n;
    l.stack = (int*) malloc((2 * n + 1) * sizeof(int));
    l.lits = (bool*) malloc((2 * n + 1) * sizeof(bool));
    if(l.lits == NULL || l.stack == NULL) {
        return false;
    }
    l.lits += n;
    l.stackused = l.stack;
    for(int i = -n; i <= n; ++i) {
        l.lits[i] = false;
    }
    return true;
}

bool reallocateDb(lrat& l) {
    l.dbmax *= 2;
    l.dbarray = (int*) realloc(l.dbarray, l.dbmax * sizeof(int));
    return l.dbarray != NULL;
}

bool copyClause(lrat& l, int*& ptr, int except) {
    int lit;
    bool taut = false;
    while((lit = *ptr++) != 0) {
        if(lit != except) {
            taut |= l.lits[-lit];
            if(!l.lits[lit]) {
                *l.stackused++ = lit;
                l.lits[lit] = true;
            }
        }
    }
    return !taut;
}

void dropModel(lrat& l) {
    int* it = l.stack;
    while(it < l.stackused) {
        l.lits[*it++] = 0;
    }
    l.stackused = l.stack;
}

void checkClause(lrat& l, int*& ptr, bool& contradiction) {
    int lit;
    ptr += 2;
    bool ignore = false;
    int unsat = 0;
    int prop = 0;
    if(!contradiction) {
        while((lit = *ptr++) != 0) {
            if(l.lits[-lit]) {
                ignore = true;
            } else {
                if(!l.lits[lit]) {
                    ++unsat;
                    prop = lit;
                }
            }
        }
        if(!ignore) {
            if(unsat == 0) {
                contradiction = true;
            }
            if(unsat == 1) {
                l.lits[-prop] = true;
                *(l.stackused++) = -prop;
            }
        }
    }
}

bool checkDoubleChain(lrat& l, int*& prechain, int*& postchain) {
    int index;
    int* clsptr;
    bool contradiction = false;
    while((index = *prechain++) > 0) {
        clsptr = l.ptrarray[index];
        if(*clsptr % 2 == 0) {
            return false;
        } else {
            checkClause(l, clsptr, contradiction);
        }
    }
    while((index = *postchain++) > 0) {
        clsptr = l.ptrarray[index];
        if(*clsptr % 2 == 0) {
            return false;
        } else {
            checkClause(l, clsptr, contradiction);
        }
    }
    return contradiction;
}

bool checkChain(lrat& l, int*& ptr) {
    int index;
    int* clsptr;
    bool contradiction = false;
    while((index = *ptr++) > 0) {
        clsptr = l.ptrarray[index];
        if(*clsptr % 2 == 0) {
            return false;
        } else {
            checkClause(l, clsptr, contradiction);
        }
    }
    return contradiction;
}

bool containsLit(int* clause, int pivot) {
    while(*clause != 0) {
        if(*clause++ == pivot) {
            return true;
        }
    }
    return false;
}

bool findChain(int*& ptr, int index) {
    while(*ptr != 0) {
        if(*ptr++ == index) {
            return true;
        }
    }
    return false;
}

bool checkRat(lrat& l, int* ptr, int*& chain, int pivot, int bound) {
    int index;
    int* clause;
    int* resclause;
    int* reschain;
    int* prechain;
    for(index = 1; index < bound; ++index) {
        clause = ptr + 2;
        resclause = l.ptrarray[index];
        if(resclause != NULL && *resclause % 2 == 1 && containsLit(resclause + 2, -pivot)) {
            resclause += 2;
            if(copyClause(l, clause, 0) && copyClause(l, resclause, -pivot)) {
                reschain = chain;
                prechain = chain;
                if(!findChain(reschain, -index) || !checkDoubleChain(l, prechain, reschain)) {
                    return false;
                }
            }
            dropModel(l);
        }
    }
    while(*chain++ != 0) {}
    return true;
}

void activateClause(lrat& l, int index) {
    *(l.ptrarray[index]) |= 1;
}

void deactivateClause(lrat& l, int index) {
    *(l.ptrarray[index]) &= (~1);
}

bool dbInsert(lrat& l, int value) {
    if(l.dbused + 1 >= l.dbmax) {
        if(!reallocateDb(l)) {
            std::cout << "s BUG" << std::endl;
            return false;
        }
    }
    l.dbarray[l.dbused++] = value;
    return true;
}

bool findNegative(int* ptr) {
    int lit;
    while((lit = *ptr++) != 0) {
        if(lit < 0) {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    lrat Data;
    std::ifstream input;
    std::string line;
    boost::smatch parsed;
    int ptrcount;
    int* clauseptr;
    int* chainptr;
    int maxint;
    int check;
    int index;
    int literal;
    bool first;
    bool refutation;
    const boost::regex headerRegex("p (.*)");
    const boost::regex word("(-?\\d+|d)(.*)?");
    const boost::regex deletion("d(.*)");
    bool parsedok;
    int mode;
    const int modeCnfNewClause = 1;
    const int modeCnfInClause = 2;
    const int modeLratIndex = 3;
    const int modeLratInClause = 4;
    const int modeLratChain = 5;
    const int modeLratInDeletion = 6;

    if(!allocate(Data)) {
        std::cout << "s BUG" << std::endl;
        return 1;
    }

    ptrcount = 0;
    maxint = 0;
    if(argc != 3) {
        std::cout << "s ERROR: invalid input arguments" << std::endl;
        return 1;
    }
    input.open(argv[1]);
    if(input.fail()) {
        std::cout << "s ERROR: invalid input arguments" << std::endl;
        return 1;
    }
    parsedok = false;
    while(input.good() && !parsedok) {
		getline(input, line);
		parsedok = boost::regex_search(line, parsed, headerRegex);
	}
    if(!parsedok) {
        std::cout << "s ERROR: invalid input arguments" << std::endl;
        return 1;
    }
    mode = modeCnfNewClause;
    first = true;
    while(input.good()) {
        getline(input, line);
        while(boost::regex_search(line, parsed, word)) {
            if(mode == modeCnfNewClause) {
                mode = modeCnfInClause;
                ++ptrcount;
                if(first) {
                    first = false;
                } else {
                    if(!dbInsert(Data, 0)) { return 1; }
                }
                if(!dbInsert(Data, 3)) { return 1; }
                if(!dbInsert(Data, ptrcount)) { return 1; }
            }
            literal = boost::lexical_cast<int>(parsed[1]);
            if(literal == 0) {
                mode = modeCnfNewClause;
                if(!dbInsert(Data, 0)) { return 1; }
                if(!dbInsert(Data, 0)) { return 1; }
            } else {
                if(abs(literal) > maxint) {
                    maxint = abs(literal);
                }
                if(!dbInsert(Data, literal)) { return 1; }
            }
            line = parsed[2];
        }
    }
    input.close();
    input.open(argv[2]);
    if(input.fail()) {
        std::cout << "s ERROR: invalid input arguments" << std::endl;
        return 1;
    }
    mode = modeLratIndex;
    refutation = false;
    while(input.good()) {
        getline(input, line);
        while(boost::regex_search(line, parsed, word)) {
            if(mode == modeLratIndex) {
                index = boost::lexical_cast<int>(parsed[1]);
                if(index == ptrcount) {
                    line = parsed[2];
                    if(!boost::regex_search(line, parsed, deletion)) {
                        std::cout << "s ERROR: invalid input arguments" << std::endl;
                        return 1;
                    }
                    line = parsed[1];
                    mode = modeLratInDeletion;
                } else if(index < ptrcount) {
                    std::cout << "s ERROR: invalid input arguments" << std::endl;
                    return 1;
                } else {
                    ptrcount = index;
                    if(!dbInsert(Data, 0)) { return 1; }
                    if(!dbInsert(Data, 0)) { return 1; }
                    if(!dbInsert(Data, ptrcount)) { return 1; }
                    line = parsed[2];
                    mode = modeLratInClause;
                    first = true;
                }
            } else if(mode == modeLratInClause) {
                literal = boost::lexical_cast<int>(parsed[1]);
                if(literal == 0) {
                    if(first) {
                        refutation = true;
                    }
                    mode = modeLratChain;
                    if(!dbInsert(Data, 0)) { return 1; }
                } else {
                    if(abs(literal) > maxint) {
                        maxint = abs(literal);
                    }
                    if(!dbInsert(Data, literal)) { return 1; }
                }
                line = parsed[2];
            } else if(mode == modeLratChain) {
                literal = boost::lexical_cast<int>(parsed[1]);
                if(literal == 0) {
                    mode = modeLratIndex;
                    if(!dbInsert(Data, 0)) { return 1; }
                } else {
                    if(literal >= ptrcount) {
                        std::cout << "s BUG" << std::endl;
                        return 1;
                    }
                    if(!dbInsert(Data, literal)) { return 1; }
                }
                line = parsed[2];
            } else if(mode == modeLratInDeletion) {
                literal = boost::lexical_cast<int>(parsed[1]);
                if(literal == 0) {
                    mode = modeLratIndex;
                } else {
                    if(!dbInsert(Data, literal)) { return 1; }
                }
                line = parsed[2];
            }
        }
    }
    if(!dbInsert(Data, 0)) { return 1; }
    input.close();

    if(!allocateLits(Data, maxint + 1) || !allocatePtr(Data, ptrcount + 3)) {
        std::cout << "s BUG" << std::endl;
        return 1;
    }

    clauseptr = Data.dbarray;
    check = 0;
    while(check == 0) {
        index = clauseptr[1];
        Data.ptrarray[index] = clauseptr;
        chainptr = clauseptr + 2;
        if(copyClause(Data, chainptr, 0)) {
            if((*clauseptr >> 1) % 2 == 0) {
                if(*chainptr != 0 && !findNegative(chainptr)) {
                    if(!checkChain(Data, chainptr)) {
                        check = 1;
                    } else {
                        dropModel(Data);
                    }
                } else {
                    dropModel(Data);
                    if(!checkRat(Data, clauseptr, chainptr, clauseptr[2], index)) {
                        check = 1;
                    }
                }
            } else {
                dropModel(Data);
                ++chainptr;
            }
        } else {
            while(*chainptr++ != 0) {}
        }
        if(check == 0) {
            activateClause(Data, index);
            while((index = *chainptr++) != 0) {
                deactivateClause(Data, index);
            }
        }
        if(chainptr - Data.dbarray == Data.dbused && refutation) {
            check = 2;
        }
        clauseptr = chainptr;
    }
    if(check == 1) {
        std::cout << "s REJECTED" << std::endl;
    } else if(check == 2) {
        std::cout << "s VERIFIED" << std::endl;
    } else {
        std::cout << "s BUG" << std::endl;
    }
    Data.lits -= Data.nolits;
    free(Data.dbarray);
    free(Data.ptrarray);
    free(Data.stack);
    free(Data.lits);
    return 0;
}
