#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"

namespace Recheck {

//----------------------
// Memory management
//----------------------

int it;

bool allocate(recheck& rc) {
    if(Parameters::recheck) {
        #ifdef VERBOSE
        Blablabla::log("Allocating rechecker");
        #endif
        rc.natlits = (bool*) malloc((2 * Stats::variableBound + 1) * sizeof(bool));
        rc.reslits = (bool*) malloc((2 * Stats::variableBound + 1) * sizeof(bool));
        if(rc.natlits == NULL || rc.reslits == NULL) {
            #ifdef VERBOSE
            Blablabla::log("Error at rechecker allocation");
            #endif
            Blablabla::comment("Memory management error");
            return false;
        } else {
            rc.natlits += Stats::variableBound;
            rc.reslits += Stats::variableBound;
            for(it = -Stats::variableBound; it <= Stats::variableBound; ++it) {
                rc.natlits[it] = false;
                rc.reslits[it] = false;
            }
        }
    }
    return true;
}

void deallocate(recheck& rc) {
    if(Parameters::recheck) {
        rc.natlits -= Stats::variableBound;
        rc.reslits -= Stats::variableBound;
        free(rc.natlits);
        free(rc.reslits);
    }
}

//----------------------
// Clausal checks
//----------------------

int lit;

void copyModel(model& m, bool* dst) {
    for(lit = -Stats::variableBound; lit <= Stats::variableBound; ++lit) {
        dst[lit] = m.lits[lit];
    }
}

bool checkPointer(int* clause, bool* model, int except) {
    while((lit = *clause++) != Constants::EndOfList) {
        if(!model[-lit] && lit != except) {
            #ifdef VERBOSE
            Blablabla::log("Model does not subsume clause complement");
            #endif
            return false;
        }
    }
    return true;
}

bool checkConsistency(bool* model) {
    for(lit = 0; lit <= Stats::variableBound; ++lit) {
        if(model[lit] && model[-lit]) {
            #ifdef VERBOSE
            Blablabla::log("Model is inconsistent");
            #endif
            return false;
        }
    }
    return true;
}

bool checkPresence(database& d, long offset) {
    if(Database::isFlag(Database::getPointer(d, offset), Constants::ActivityBit, Constants::ActiveFlag)) {
        return true;
    } else {
        #ifdef VERBOSE
        Blablabla::log("Resolution clause does not occur in accumulated formula");
        #endif
        return false;
    }
}

bool checkClause(recheck& rc, database& d) {
    return checkConsistency(rc.natlits) && checkPointer(Database::getPointer(d, rc.clause), rc.natlits, Constants::EndOfList);
}

bool checkResolvent(recheck& rc, database& d) {
    return checkConsistency(rc.reslits) &&
            checkPresence(d, rc.resolvent) &&
            checkPointer(Database::getPointer(d, rc.clause), rc.reslits, rc.pivot) &&
            checkPointer(Database::getPointer(d, rc.resolvent), rc.reslits, -rc.pivot);
}




}
