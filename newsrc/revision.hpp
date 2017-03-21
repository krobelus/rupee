#ifndef __REVISION_H_INCLUDED__
#define __REVISION_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace Revision {
    bool allocate(revision& v);
    void deallocate(revision& v);
}

#endif
