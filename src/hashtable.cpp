#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"
#include "database.hpp"
#include "clause.hpp"
#include "hashtable.hpp"

namespace HashTable {
    bool error;
    unsigned int sum;
	unsigned int prod;
	unsigned int xoor;
    long* list;
    int length;
    int* ptr;
    int it;

    bool allocate(hashtable& h) {
        error = false;
    	Blablabla::log("Allocating hash table.");
    	h.rowarray = (long**) malloc (Parameters::databaseSize * sizeof (long*));
    	h.rowused = (int*) malloc (Parameters::databaseSize * sizeof (int));
    	h.rowmax = (int*) malloc (Parameters::databaseSize * sizeof (int));
    	if(h.rowarray == NULL || h.rowused == NULL || h.rowmax == NULL) {
    		error = true;
    	}
    	for(it = 0; it < Parameters::databaseSize && !error; ++it) {
    		h.rowarray[it] = (long*) malloc (Parameters::hashDepth * sizeof (long));
    		h.rowmax[it] = Parameters::hashDepth;
    		h.rowused[it] = 0;
    		if(h.rowarray[it] == NULL) {
    			error = true;
    		}
    	}
    	if(error) {
    		Blablabla::log("Error at hash table allocation.");
    		Blablabla::comment("Memory management error.");
    		return false;
    	} else {
    		return true;
    	}
    }

    bool reallocateRow(hashtable& h, int row) {
        Blablabla::log("Reallocating hash table.");
        h.rowmax[row] = (h.rowmax[row] * 3) >> 1;
        h.rowarray[row] = (long*) realloc(h.rowarray[row], h.rowmax[row] * sizeof(long));
        if(h.rowarray[row] == NULL) {
            Blablabla::log("Error at hash table reallocation.");
            Blablabla::comment("Memory management error.");
            return false;
        } else {
            return true;
        }
    }

	void deallocate(hashtable& h) {
        Blablabla::log("Deallocating hash table.");
    	for(it = 0; it < Parameters::databaseSize; ++it) {
    		free(h.rowarray[it]);
    	}
    	free(h.rowarray);
    	free(h.rowused);
    	free(h.rowmax);
    }

    unsigned int getHash(int* ptr) {
        sum = 0;
        prod = 1;
        xoor = 0;
        while(*ptr) {
            prod *= *ptr;
            sum += *ptr;
            xoor ^= *ptr;
            ++ptr;
        }
        return (((1023 * sum + prod) ^ (31 * xoor))) % Parameters::databaseSize;
    }

    bool match(hashtable& h, database& d, clause& c, unsigned int hash, long& offset) {
        list = h.rowarray[hash];
        length = h.rowused[hash];
        for(it = 0; it < length; ++it) {
            ptr = Database::getPointer(d, list[it]);
            if(Database::isFlag(ptr, Constants::RawActivityBit, !c.kind)) {
                if(Clause::equals(c, ptr)) {
                    offset = list[it];
                    return true;
                }
            }
        }
        return false;
    }

    bool insertOffset(hashtable& h, unsigned int hash, long offset) {
        if(h.rowused[hash] >= h.rowmax[hash]) {
            if(!reallocateRow(h, hash)) { return false; }
        }
        h.rowarray[hash][h.rowused[hash]++] = offset;
        return true;
    }

    bool removeOffset(hashtable& h, unsigned int hash, long offset) {
        list = h.rowarray[hash];
        length = h.rowused[hash];
        for(it = 0; it < length; ++it) {
            if(list[it] == offset) {
                list[it] = list[--(h.rowused[hash])];
                return true;
            }
        }
        return false;
    }
}
