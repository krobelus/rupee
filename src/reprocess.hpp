#ifndef __REPROCESS_H_INCLUDED__
#define __REPROCESS_H_INCLUDED__

namespace Reprocess {
    extern int* reprocessStack;
    extern int* reprocessUsed;
    extern bool* unpropagations;
    extern int* reprocessOld;

    extern int* stackLeft_;
    extern int* stackRight_;
    extern int literal_;
    extern int* clause_;
    extern bool currentFound_;
    extern bool currentInserted_;
    extern int checkLiteral_;
    extern int watchIterator_;

    void initialize();
    void deallocate();

    void recordUnpropagation(int literal);
    bool mustUnpropagate(int* &clause, int literal);
    void cleanUnpropagations();

    void unpropagateClause(long offset);

    bool reprocess();
}

#endif
