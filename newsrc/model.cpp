#include <stdlib.h>
#include <string>

#include "structs.hpp"
#include "parser.hpp"
#include "extra.hpp"
#include "model.hpp"

namespace Model {

int it;

bool allocate(model& m) {
    Blablabla::log("Allocating model.");
    m.start = (int*) malloc((2 * Parameters::noVariables + 1) * sizeof(int));
    m.satisfied = (bool*) malloc((2 * Parameters::noVariables + 1) * sizeof(bool));
    m.positions = (int**) malloc((2 * Parameters::noVariables + 1) * sizeof(int*));
    if(m.start == NULL || m.positions == NULL || m.satisfied == NULL) {
        Blablabla::log("Error at model allocation.");
		Blablabla::comment("Memory management error.");
        return false;
    }
    m.forced = m.head = m.used;
    m.satisfied += Parameters::noVariables;
    m.positions += Parameters::noVariables;
    for(it = 0; it <= Parameters::noVariables; ++it) {
        m.satisfied[it] = m.satisfied[-it] = false;
        m.positions[it] = m.positions[-it] = NULL;
    }
    return true;
}

void deallocate(model& t) {
    Blablabla::log("Deallocating model.");
    m.satisfied -= Parameters::noVariables;
    m.positions -= Parameters::noVariables;
    free(m.start);
    free(m.satisfied);
    free(m.positions);
}




}
