#ifndef __DATABASE_H_INCLUDED__
#define __DATABASE_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace Database {
    extern int it;

    bool allocate(database& d);
    bool reallocate(database& d);
    void deallocate(database& d);
    long getOffset(database& d, int* ptr);
    int* getPointer(database& d, long offset);
    bool isFlag(int* ptr, int bit, bool value);
    void setFlag(int* ptr, int bit, bool value);
    int classifyClause(int* ptr);
    bool addBufferClause(database& d, clause& c, long& offset);
    void setPremiseFlags(int* ptr);
    void setIntroductionFlags(int* ptr);
    void setDeletionFlags(int* ptr);
    void log(database& d);
    std::string offsetToString(database& d, long offset);
}

#endif
