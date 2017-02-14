#include <stdlib.h>
#include <string>

#include "parser.hpp"
#include "trail.hpp"

namespace Trail {

int it;

bool allocate(parser& p, trail& t) {
    Blablabla::log("Allocating trail.");
    t.Stack.array = (int*) malloc((2 * p.maxVariable + 1) * sizeof(int));
    t.Stack.current = t.Stack.used = t.Stack.array;
    t.Assignment.satisfied = (bool*) malloc((2 * p.maxVariable + 1) * sizeof(bool));
    t.Assignment.trailpositions = (int**) malloc((2 * p.maxVariable + 1) * sizeof(int*));
    t.Assignment.reasons = (long*) malloc((2 * p.maxVariable + 1) * sizeof(long));
    if(t.Stack.array == NULL || t.Assignment.satisfied == NULL || t.Assignment.trailpositions == NULL || t.Assignment.reasons == NULL) {
        Blablabla::log("Error at trail allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    }
    t.Assignment.satisfied += p.maxVariable;
    t.Assignment.trailpositions += p.maxVariable;
    t.Assignment.reasons += p.maxVariable;
    for(it = 0; it < p.maxVariable; ++it) {
        t.Assignment.satisfied[it] = t.Assignment.satisfied[-it] = false;
    }
    return true;
}

void deallocate(parser& p, trail& t) {
    Blablabla::log("Deallocating trail.");
    free(t.Stack.array);
    free(t.Assignment.satisfied);
    free(t.Assignment.trailpositions);
    free(t.Assignment.reasons);
}




}
