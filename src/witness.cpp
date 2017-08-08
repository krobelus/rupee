#include <stdlib.h>
#include "structs.hpp"
#include "extra.hpp"

namespace Witness {

//-------------------------------
// Memory management
//-------------------------------

int it;

bool allocate(witness& wt) {
    #ifdef VERBOSE
    Blablabla::log("Allocating witness");
    #endif
    wt.max = Parameters::databaseSize;
    wt.used = 0;
    wt.array = (long*) malloc(wt.max * sizeof(long));
    wt.lits = (bool*) malloc((2 * Stats::variableBound + 1) * sizeof(bool));
    wt.chain = (int*) malloc((2 * Stats::variableBound + 1) * sizeof(int));
    wt.head = wt.last = wt.chain;
    if(wt.array == NULL || wt.lits == NULL || wt.chain == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at witness allocation");
        #endif
        Blablabla::comment("Memory management error");
        return false;
    }
    wt.lits += Stats::variableBound;
    for(it = -Stats::variableBound; it <= Stats::variableBound; ++it) {
        wt.lits[it] = false;
    }
    closeWitness(wt);
    return true;
}

bool reallocate(witness& wt) {
    #ifdef VERBOSE
    Blablabla::log("Reallocating witness");
    #endif
    wt.max *= 2;
    wt.array = (long*) realloc(wt.array, wt.max * sizeof(long));
    if(wt.array == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at witness allocation");
        #endif
        Blablabla::comment("Memory management error");
        return false;
    }
    return true;
}

void deallocate(witness& wt) {
    #ifdef VERBOSE
    Blablabla::log("Deallocating witness");
    #endif
    wt.lits -= Stats::variableBound;
    free(wt.array);
    free(wt.lits);
    free(wt.chain);
}

//-------------------------------
// Witness construction
//-------------------------------

long off;

bool openWitness(witness& wt, long offset) {
    if(wt.used + 1 >= wt.max) {
        if(!reallocate(wt)) { return false; }
    }
    wt.array[wt.used++] = offset;
    return true;
}

bool setRupWitness(witness& wt) {
    return setRatWitness(wt, Constants::ConflictLiteral);
}

bool setRatWitness(witness& wt, int literal) {
    if(wt.used + 1 >= wt.max) {
        if(!reallocate(wt)) { return false; }
    }
    wt.array[wt.used++] = (long) literal;
    wt.array[wt.used++] = (long) Constants::EndOfList;
    return true;
}

bool dumpAndSchedule(witness& wt, model& m, database& d) {
    // while(wt.used + (wt.last - wt.chain) + 1 >= wt.max) {
    //     if(!reallocate(wt)) { return false; }
    // }
    // wt.head = wt.chain;
    // while(wt.head < wt.last) {
    //     off = m.reason[*(wt.head++)];
    //     wt.array[wt.used++] = off;
    //     Database::setFlag(Database::getPointer(d, off), Constants::VerificationBit, Constants::ScheduledFlag);
    // }
    while(wt.used + (wt.last - wt.chain) + 1 >= wt.max) {
        if(!reallocate(wt)) { return false; }
    }
    wt.head = wt.last;
    while(wt.chain <= --wt.head) {
        off = m.reason[*wt.head];
        wt.array[wt.used++] = off;
        Database::setFlag(Database::getPointer(d, off), Constants::VerificationBit, Constants::ScheduledFlag);
    }
    return true;
}

bool setResolventWitness(witness& wt, long offset) {
    if(wt.used >= wt.max) {
        if(!reallocate(wt)) { return false; }
    }
    wt.array[wt.used++] = -offset;
    return true;
}

bool closeWitness(witness& wt) {
    if(wt.used >= wt.max) {
        if(!reallocate(wt)) { return false; }
    }
    wt.array[wt.used++] = Constants::EndOfList;
    return true;
}

bool recordDeletion(witness& wt, long offset) {
    if(wt.used >= wt.max) {
        if(!reallocate(wt)) { return false; }
    }
    wt.array[wt.used++] = -offset;
    return true;
}
//
// //-------------------------------
// // Witness extraction
// //-------------------------------

long* extptr;
long extoff;
int* extclause;
bool extmode;
std::string extstr;
bool extpremise;
std::ofstream output;

void extractWitness(witness& wt, database& d) {
    if(Parameters::generateLrat) {
        extptr = wt.array + wt.used;
        extmode = Constants::InstructionIntroduction;
        output.open(Parameters::pathWitness, std::ofstream::out | std::ofstream::trunc);
        while(stepBack(wt, extptr, extmode)) {
            if(extmode == Constants::InstructionIntroduction) {
                extclause = Database::getPointer(d, *extptr);
                if((extpremise = Database::isFlag(extclause, Constants::OriginalityBit, Constants::DerivedFlag)) || Parameters::gritMode) {
                    if(extpremise) {
                        extclause[Constants::IdCellDatabase] = ++(d.idCount);
                    }
                    extstr = std::to_string(extclause[Constants::IdCellDatabase]) + " ";
                    extractClause(extclause, (int) extptr[1], extstr);
                    extractChain(d, extptr + 3, extstr);
                    output << extstr << std::endl;
                }
            } else {
                if(*extptr != Constants::EndOfList) {
                    extstr = std::to_string(d.idCount) + " d ";
                    while(*extptr != Constants::EndOfList) {
                        extclause = Database::getPointer(d, -*extptr);
                        if(Database::isFlag(extclause, Constants::VerificationBit, Constants::ScheduledFlag) ||
                                Database::isFlag(extclause, Constants::OriginalityBit, Constants::PremiseFlag)) {
                            extstr += std::to_string(extclause[Constants::IdCellDatabase]) + " ";
                        }
                        --extptr;
                    }
                    extstr += "0";
                    output << extstr << std::endl;
                }
            }
        }
        output.close();
        Blablabla::comment("c LRAT proof written to " + Parameters::pathWitness);
    }
}

bool stepBack(witness& wt, long*& wtcell, bool& wtmd) {
    if(wtmd == Constants::InstructionIntroduction) {
        --wtcell;
        if(wtcell <= wt.array) {
            return false;
        }
    } else {
        while(*(--wtcell) != Constants::EndOfList) {}
        wtcell -= 2;
    }
    wtmd = !wtmd;
    return true;
}

void extractClause(int*& cls, int piv, std::string& str) {
    if(piv != Constants::EndOfList) {
        str += Blablabla::litToString(piv) + " ";
    }
    while(*cls != Constants::EndOfList) {
        if(*cls != piv && *cls != -Constants::ReservedLiteral) {
            str += Blablabla::litToString(*cls) + " ";
        }
        cls++;
    }
    str += "0 ";
}

void extractChain(database& d, long* lptr, std::string& str) {
    while((extoff = *lptr++) != Constants::EndOfList) {
        if(extoff > 0) {
            str += std::to_string((Database::getPointer(d, extoff))[Constants::IdCellDatabase]) + " ";
        } else {
            str += std::to_string(-((Database::getPointer(d, -extoff))[Constants::IdCellDatabase])) + " ";
        }
    }
    str += "0";
}


//-------------------------------
// Chain generation
//-------------------------------

int pivot;
int lit;
int* ptr;

void extractDependencies(witness& wt, model& m, database& d, int literal) {
    resetChain(wt);
    addChainLiteral(wt, literal);
    wt.head = wt.chain;
    while(wt.head < wt.last) {
        findShift(wt, m, d);
    }
    sortChain(wt, m);
    #ifdef VERBOSE
    Blablabla::logChain(wt, m, d);
    #endif
}

void findShift(witness& wt, model& m, database& d) {
    pivot = *(wt.head++);
    ptr = Database::getPointer(d, m.reason[pivot]);
    while((lit = *(ptr++)) != Constants::EndOfList) {
        if(lit != pivot && !wt.lits[-lit] && m.reason[-lit] != Constants::AssumedReason) {
            addChainLiteral(wt, -lit);
        }
    }
}

void resetChain(witness& wt) {
    wt.head = wt.last;
    while(wt.head > wt.chain) {
        wt.lits[*(--wt.head)] = false;
    }
    wt.last = wt.chain;
}



//-------------------------------
// Low-level commands
//-------------------------------

int** pos;
int* c1;
int* c2;

void addChainLiteral(witness& wt, int literal) {
    wt.lits[literal] = true;
    *(wt.last++) = literal;
}

void sortChain(witness& wt, model& m) {
    pos = m.position;
    qsort(wt.chain, wt.last - wt.chain, sizeof(int), compare);
}

int compare(const void *a, const void *b) {
    c1 = pos[*(int*) a];
    c2 = pos[*(int*) b];
    return (int)(c1 < c2) - (int)(c2 < c1);
}

}
