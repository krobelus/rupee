#include <stdlib.h>
#include <string>

#include "parser.hpp"
#include "extra.hpp"
#include "trail.hpp"

namespace Trail {

int it;

bool allocate(trail& t) {
    Blablabla::log("Allocating trail.");
    t.Stack.array = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    t.Stack.current = t.Stack.used = t.Stack.array;
    t.Assignment.satisfied = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    t.Assignment.trailpositions = (int**) malloc((2 * Parameters::noVariables + 1) * sizeof(int*));
    t.Assignment.reasons = (long*) malloc((2 * Parameters::noVariables + 1) * sizeof(long));
    if(t.Stack.array == NULL || t.Assignment.satisfied == NULL || t.Assignment.trailpositions == NULL || t.Assignment.reasons == NULL) {
        Blablabla::log("Error at trail allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    }
    t.Assignment.satisfied += Parameters::noVariables;
    t.Assignment.trailpositions += Parameters::noVariables;
    t.Assignment.reasons += Parameters::noVariables;
    for(it = 0; it < Parameters::noVariables; ++it) {
        t.Assignment.satisfied[it] = t.Assignment.satisfied[-it] = false;
    }
    return true;
}

void deallocate(trail& t) {
    Blablabla::log("Deallocating trail.");
    free(t.Stack.array);
    free(t.Assignment.satisfied);
    free(t.Assignment.trailpositions);
    free(t.Assignment.reasons);
}




}
