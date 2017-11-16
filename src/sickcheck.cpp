#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <sys/time.h>

struct sickcheck {
    bool* natmodel;
    bool* resmodel;
    int*  natclause;
    int* resclause;
    int* dbarray;
    int dbused;
    int dbmax;
    long** hasharray;
    int* hashused;
    int* hashmax;
    int variables;
    int pivot;
    int instruction;
    std::string cnf;
    std::string drat;
    std::string sick;
    int state;
    int counter;
} Sickcheck;

boost::regex HeaderRegex("p(.*)");
boost::regex CommentRegex("c(.*)");
boost::regex WordRegex("(-?\\d+|d)(.*)?");
boost::regex DeletionRegex("d(.*)");
boost::regex VariablesRegex("v(.*)");
boost::regex NaturalRegex("n(.*)");
boost::regex ResolventRegex("r(.*)");

void readArgs(int argc, char* argv[]) {
    Sickcheck.natmodel = NULL;
    Sickcheck.resmodel = NULL;
    Sickcheck.natclause = NULL;
    Sickcheck.resclause = NULL;
    Sickcheck.dbarray = NULL;
    Sickcheck.hasharray = NULL;
    Sickcheck.hashmax = NULL;
    if(Sickcheck.state >= 0) {
        std::string stringArgument;
        if(argc < 3) {
            std::cout << "Incorrect arguments" << std::endl;
            Sickcheck.state = -1;
            return;
        }
        Sickcheck.cnf = argv[1];
        Sickcheck.drat = argv[2];
        Sickcheck.sick = argv[3];
    }
}

void allocate() {
    Sickcheck.dbused = 0;
    Sickcheck.dbarray = (int*) malloc(Sickcheck.dbmax * 2 * sizeof(int));
    Sickcheck.hasharray = (long**) malloc(Sickcheck.dbmax / 100 * sizeof(long*) );
    Sickcheck.hashused = (int*) malloc(Sickcheck.dbmax / 100 * sizeof(int));
    Sickcheck.hashmax = (int*) malloc(Sickcheck.dbmax / 100 * sizeof(int));
    for(int i = 0; i < Sickcheck.dbmax / 100; ++i) {
        Sickcheck.hasharray[i] = (long*) malloc(100 * sizeof(long));
        Sickcheck.hashused[i] = 0;
        Sickcheck.hashmax[i] = 100;
    }
}

void badParsing(std::string str) {
    std::cout << "Could not parse " << str << " file" << std::endl;
    Sickcheck.state = -1;
}

void allocateVars() {
    Sickcheck.natmodel = (bool*) malloc( (2 * Sickcheck.variables + 1) * sizeof(bool) );
    Sickcheck.resmodel = (bool*) malloc( (2 * Sickcheck.variables + 1) * sizeof(bool) );
    Sickcheck.natclause = (int*) malloc( (Sickcheck.variables + 1) * sizeof(int) );
    Sickcheck.resclause = (int*) malloc( (Sickcheck.variables + 1) * sizeof(int) );
    Sickcheck.resmodel += Sickcheck.variables;
    Sickcheck.natmodel += Sickcheck.variables;
    for(int i = -Sickcheck.variables; i <= Sickcheck.variables; ++i) {
        Sickcheck.natmodel[i] = false;
        Sickcheck.resmodel[i] = false;
    }
}

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void parseSick() {
    if(Sickcheck.state >= 0) {
        std::ifstream input;
        std::string line;
        bool parsing;
        int literal;
        boost::smatch parsed;
        int* pointer;

        input.open(Sickcheck.sick);
        if(input.fail()) {
            std::cout << "Could not open SICK file" << std::endl;
            Sickcheck.state = -1;
            return;
        }
        getline(input, line);
        if(!boost::regex_search(line, parsed, VariablesRegex)) { badParsing("SICK"); input.close(); return; }
        line = parsed[1];
        if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
        Sickcheck.variables = boost::lexical_cast<int>(parsed[1]);
        std::cout << "Variables: " << Sickcheck.variables << std::endl;
        allocateVars();
        line = parsed[2];
        if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
        Sickcheck.pivot = boost::lexical_cast<int>(parsed[1]);
        std::cout << "Pivot: " << Sickcheck.pivot << std::endl;
        line = parsed[2];
        if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
        Sickcheck.dbmax = boost::lexical_cast<int>(parsed[1]);
        std::cout << "Database size: " << Sickcheck.dbmax << std::endl;
        line = parsed[2];
        if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
        Sickcheck.instruction = boost::lexical_cast<int>(parsed[1]);
        std::cout << "Instruction: " << Sickcheck.instruction << std::endl;

        getline(input, line);
        if(!boost::regex_search(line, parsed, NaturalRegex)) { badParsing("SICK"); input.close(); return; }
        line = parsed[1];
        parsing = true;
        pointer = Sickcheck.natclause;
        std::cout << "Parsing natural clause" << std::endl;
        while(parsing) {
            if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
            literal =  boost::lexical_cast<int>(parsed[1]);
            *pointer++ = literal;
            line = parsed[2];
            if(literal == 0) {
                std::sort(Sickcheck.natclause, pointer - 1);
                parsing = false;
            }
        }
        std::cout << "Parsing natural model" << std::endl;
        parsing = true;
        while(parsing) {
            if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
            literal =  boost::lexical_cast<int>(parsed[1]);
            if(literal == 0) {
                parsing = false;
            } else {
                Sickcheck.natmodel[literal] = true;
            }
            line = parsed[2];
        }
        getline(input, line);
        if(!boost::regex_search(line, parsed, ResolventRegex)) { badParsing("SICK"); input.close(); return; }
        line = parsed[1];
        parsing = true;
        pointer = Sickcheck.resclause;
        std::cout << "Parsing resolution clause" << std::endl;
        while(parsing) {
            if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
            literal =  boost::lexical_cast<int>(parsed[1]);
            *pointer++ = literal;
            line = parsed[2];
            if(literal == 0) {
                std::sort(Sickcheck.resclause, pointer - 1);
                parsing = false;
            }
        }
        parsing = true;
        std::cout << "Parsing resolution model" << std::endl;
        while(parsing) {
            if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("SICK"); input.close(); return; }
            literal =  boost::lexical_cast<int>(parsed[1]);
            if(literal == 0) {
                parsing = false;
            } else {
                Sickcheck.resmodel[literal] = true;
            }
            line = parsed[2];
        }
        input.close();
    }
}

unsigned int getHash(int* ptr) {
    unsigned int sum = 0;
    unsigned int prod = 1;
    unsigned int xoor = 0;
    int literal;
    while((literal = *ptr++) != 0) {
        sum += literal;
        prod *= literal;
        xoor ^= literal;
    }
    return ((1023 * sum + prod) ^ (31 * xoor)) % (Sickcheck.dbmax / 100);
}

void parseCnf() {
    if(Sickcheck.state >= 0) {
        std::ifstream input;
        std::string line;
        bool parsing;
        boost::smatch parsed;
        int* clause;
        int* nextclause;
        int literal;
        unsigned int hash;

        Sickcheck.counter = 0;
        std::cout << "Parsing CNF formula" << std::endl;
        clause = (int*) malloc((Sickcheck.variables + 1) * sizeof(int));
        input.open(Sickcheck.cnf);
        if(input.fail()) {
            std::cout << "Could not open CNF file" << std::endl;
            Sickcheck.state = -1;
            return;
        }
        parsing = true;
        while(parsing) {
            if(!input.good()) { badParsing("CNF"); /*free(&clause);*/ input.close(); return; }
            getline(input, line);
            if(boost::regex_search(line, parsed, HeaderRegex)) {
                parsing = false;
            }
        }
        while(true) {
            if(!input.good()) {
                input.close();
                return;
            }
            getline(input, line);
            nextclause = clause;
            parsing = !line.empty();
            while(parsing) {
                if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("CNF"); /*free(&clause);*/ input.close(); return; }
                literal = boost::lexical_cast<int>(parsed[1]);
                *nextclause++ = literal;
                line = parsed[2];
                if(literal == 0) {
                    ++Sickcheck.counter;
                    if(Sickcheck.counter == Sickcheck.instruction) {
                        Sickcheck.state = -1;
                        input.close();
                        return;
                    }
                    std::sort(clause, nextclause - 1);
                    hash = getHash(clause);
                    if(Sickcheck.hashused[hash] + 5 >= Sickcheck.hashmax[hash]) {
                        Sickcheck.hashmax[hash] *= 2;
                        Sickcheck.hasharray[hash] = (long*) realloc(Sickcheck.hasharray[hash], Sickcheck.hashmax[hash] * sizeof(long));
                    }
                    Sickcheck.dbarray[Sickcheck.dbused++] = 1;
                    Sickcheck.hasharray[hash][Sickcheck.hashused[hash]++] = Sickcheck.dbused;
                    std::copy(clause, nextclause, Sickcheck.dbarray + Sickcheck.dbused);
                    Sickcheck.dbused += nextclause - clause;
                    parsing = false;
                }
            }
        }
    }
}

bool equalClause(int* a, int* b) {
    int i = 0;
    while(a[i] != 0) {
        if(a[i] != b[i]) {
            return false;
        }
        ++i;
    }
    return b[i] == 0;
}

bool hashFind(int* clause, unsigned int hash, int& hashpos) {
    int* other;
    hashpos = 0;
    while(hashpos < Sickcheck.hashused[hash]) {
        other = Sickcheck.dbarray + Sickcheck.hasharray[hash][hashpos];
        if(other[-1] == 1 && equalClause(clause, other)) {
            return true;
        }
        ++hashpos;
    }
    return false;
}

void parseDrat() {
    if(Sickcheck.state >= 0) {
        std::ifstream input;
        std::string line;
        bool parsing;
        boost::smatch parsed;
        int* clause;
        int* nextclause;
        int literal;
        unsigned int hash;
        bool deletion;
        int hashpos;

        std::cout << "Parsing DRAT proof" << std::endl;
        clause = (int*) malloc((Sickcheck.variables + 1) * sizeof(int));
        input.open(Sickcheck.drat);
        if(input.fail()) {
            std::cout << "Could not open DRAT file" << std::endl;
            Sickcheck.state = -1;
            return;
        }
        while(true) {
            if(!input.good()) {
                std::cout << "Instruction not found in DRAT file" << std::endl;
                Sickcheck.state = -1;
                input.close();
                return;
            }
            getline(input, line);
            if(!line.empty()) {
                nextclause = clause;
                if((deletion = boost::regex_search(line, parsed, DeletionRegex))) {
                    line = parsed[1];
                }
                parsing = true;
                while(parsing) {
                    if(!boost::regex_search(line, parsed, WordRegex)) { badParsing("DRAT"); /*free(&clause);*/  input.close(); return; }
                    literal = boost::lexical_cast<int>(parsed[1]);
                    *nextclause++ = literal;
                    line = parsed[2];
                    if(literal == 0) {
                        ++Sickcheck.counter;
                        if(Sickcheck.counter % 1000000 == 0) {
                            std::cout << " >>> " << Sickcheck.counter << std::endl;
                        }
                        std::sort(clause, nextclause - 1);
                        hash = getHash(clause);
                        if(deletion) {
                            if(Sickcheck.counter == Sickcheck.instruction) {
                                std::cout << "Instruction corresponds to a deletion" << std::endl;
                                input.close();
                                Sickcheck.state = -1;
                                return;
                            }
                            if(hashFind(clause, hash, hashpos)) {
                                Sickcheck.dbarray[Sickcheck.hasharray[hash][hashpos] - 1] = 0;
                                Sickcheck.hasharray[hash][hashpos] = Sickcheck.hasharray[hash][--Sickcheck.hashused[hash]];
                            } else {
                                std::cout << "Missed deletion" << std::endl;
                            }
                        } else {
                            if(Sickcheck.counter == Sickcheck.instruction) {
                                if(!equalClause(Sickcheck.natclause, clause)) {
                                    std::cout << "Natural clause does not correspond to instruction" << std::endl;
                                    Sickcheck.state = -1;
                                } else {
                                    std::cout << "Natural clause corresponds to instruction" << std::endl;
                                }
                                input.close();
                                return;
                            } else {
                                if(Sickcheck.hashused[hash] + 5 >= Sickcheck.hashmax[hash]) {
                                    Sickcheck.hashmax[hash] *= 2;
                                    Sickcheck.hasharray[hash] = (long*) realloc(Sickcheck.hasharray[hash], Sickcheck.hashmax[hash] * sizeof(long));
                                }
                                Sickcheck.dbarray[Sickcheck.dbused++] = 1;
                                Sickcheck.hasharray[hash][Sickcheck.hashused[hash]++] = Sickcheck.dbused;
                                std::copy(clause, nextclause, Sickcheck.dbarray + Sickcheck.dbused);
                                Sickcheck.dbused  += nextclause - clause;
                            }
                        }
                        parsing = false;
                    }
                }
            }
        }
    }
}

void checkResolvent() {
    if(Sickcheck.state >= 0 && Sickcheck.pivot != 0) {
        bool* lits;
        unsigned int hash;
        int hashpos;
        int* pointer;
        int literal;

        hash = getHash(Sickcheck.resclause);
        if(!hashFind(Sickcheck.resclause, hash, hashpos)) {
            std::cout << "Resolution candidate does not occur in the formula" << std::endl;
            Sickcheck.state = -1;
            return;
        }
        lits = (bool*) malloc((Sickcheck.variables * 2 + 1) * sizeof(bool));
        lits += Sickcheck.variables;
        for(int i = -Sickcheck.variables; i <= Sickcheck.variables; ++i) {
            lits[i] = false;
        }
        pointer = Sickcheck.natclause;
        while((literal = *pointer++) != 0) {
            if(literal != Sickcheck.pivot) {
                lits[literal] = true;
                if(lits[-literal]) {
                    std::cout << "Resolution candidate is a tautology" << std::endl;
                    Sickcheck.state = -1;
                    lits -= Sickcheck.variables;
                    return;
                }
            }
        }
        pointer = Sickcheck.resclause;
        while((literal = *pointer++) != 0) {
            if(literal != -Sickcheck.pivot) {
                lits[literal] = true;
                if(lits[-literal]) {
                    std::cout << "Resolution candidate is a tautology" << std::endl;
                    Sickcheck.state = -1;
                    lits -= Sickcheck.variables;
                    return;
                }
            }
        }
        lits -= Sickcheck.variables;
    }
}

void checkConsistency() {
    if(Sickcheck.state >= 0) {
        for(int i = 1; i <= Sickcheck.variables; ++i) {
            if(Sickcheck.natmodel[i] && Sickcheck.natmodel[-i]) {
                std::cout << "Natural model is inconsistent" << std::endl;
                Sickcheck.state = -1;
                return;
            }
            if(Sickcheck.pivot != 0 && Sickcheck.resmodel[i] && Sickcheck.resmodel[-i]) {
                std::cout << "Resolvent model is inconsistent" << std::endl;
                Sickcheck.state = -1;
                return;
            }
        }
        std::cout << "Natural model is consistent" << std::endl << "Resolvent model is consistent" << std::endl;
    }
}

bool upSatisfy(bool* model, int* clause) {
    int unknown = 0;
    int* it = clause;
    if(clause[-1] == 0) {
        return true;
    }
    while(*it != 0) {
        if(model[*it]) {
            return true;
        } else if(!model[-*it]) {
            ++unknown;
        }
        ++it;
    }
    return unknown >= 2;
}

void checkStability() {
    if(Sickcheck.state >= 0) {
        int i = 1;
        while(i < Sickcheck.dbused) {
            if(!upSatisfy(Sickcheck.natmodel, Sickcheck.dbarray + i)) {
                for(int j = i-1; Sickcheck.dbarray[j] != 0; j++) {
                    std::cout << Sickcheck.dbarray[j] << " ";
                }
                std::cout << std::endl;
                std::cout << "offset " << i << std::endl;
                std::cout << "Natural model is not a UP model of the formula" << std::endl;
                Sickcheck.state = -1;
                return;
            }
            if(Sickcheck.pivot != 0 && !upSatisfy(Sickcheck.resmodel, Sickcheck.dbarray + i)) {
                std::cout << "Resolvent model is not a UP model of the formula" << std::endl;
                Sickcheck.state = -1;
                return;
            }
            while(Sickcheck.dbarray[i] != 0) {
                ++i;
            }
            i += 2;
        }
        std::cout << "Natural model is a UP model of the formula" << std::endl << "Resolvent model is a UP model of the formula" << std::endl;
    }
}

void checkSubsumption() {
    if(Sickcheck.state >= 0) {
        for(int i = 0; Sickcheck.natclause[i] != 0; ++i) {
            if(!Sickcheck.natmodel[-Sickcheck.natclause[i]]) {
                std::cout << "Natural clause complement is not subsumed by natural model" << std::endl;
                Sickcheck.state = -1;
                return;
            }
            if(Sickcheck.pivot != 0 && Sickcheck.natclause[i] != Sickcheck.pivot && !Sickcheck.resmodel[-Sickcheck.natclause[i]]) {
                std::cout << "Resolvent clause complement is not subsumed by resolvent model" << std::endl;
                Sickcheck.state = -1;
                return;
            }
        }
        for(int i = 0; Sickcheck.resclause[i] != 0; ++i) {
            if(Sickcheck.pivot != 0 && Sickcheck.resclause[i] != -Sickcheck.pivot && !Sickcheck.resmodel[-Sickcheck.resclause[i]]) {
                std::cout << "Resolvent clause complement is not subsumed by resolvent model" << std::endl;
                Sickcheck.state = -1;
                return;
            }
        }
        std::cout << "Natural clause complement is subsumed by natural model" << std::endl;
        std::cout << "Resolvent clause complement is subsumed by resolvent model" << std::endl;
    }
}

void output() {
    if(Sickcheck.state == 0) {
        std::cout << "s VERIFIED" << std::endl;
    } else {
        std::cout << "s INCORRECT" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    struct timeval startTime;
    struct timeval endTime;
    gettimeofday (&startTime, NULL);
    Sickcheck.state = 0;
    readArgs(argc, argv);
    parseSick();
    allocate();
    parseCnf();
    parseDrat();
    checkResolvent();
    checkConsistency();
    checkStability();
    checkSubsumption();
    output();
    gettimeofday (&endTime, NULL);
    std::cout << "t " << (((endTime.tv_sec  - startTime.tv_sec) * 1000L) + ((endTime.tv_usec - startTime.tv_usec) / 1000L)) << std::endl;
}

// v VAR PIVOT DBSIZE INSTRUCTION
// n CLAUSE 0 MODEL 0
// r CLAUSE 0 MODEL 0
