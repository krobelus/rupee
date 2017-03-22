#ifndef __CHAIN_H_INCLUDED__
#define __CHAIN_H_INCLUDED__

#include "chain.hpp"
#include "structs.hpp"

namespace Chain {

bool allocate(chain& ch);
void deallocate(chain& ch);
void initialize(chain& ch, model &m, long conflict);
void addClause(chain& ch, int* ptr, int trigger);
bool getReason(chain& ch, model& m, int trigger, long& reason);
void getChain(chain& ch, model& m, database& d, long conflict);
int compare(const void *a, const void *b);
void markChain(chain& ch, database& d);

}

#endif
