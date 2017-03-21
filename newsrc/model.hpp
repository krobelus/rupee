#ifndef __MODEL_H_INCLUDED__
#define __MODEL_H_INCLUDED__

#include <stdlib.h>

#include "structs.hpp"

namespace Model {
    bool allocate(model& m);
    void deallocate(model& m);

    void assumeLiteral(model& m, int literal);
    void propagateLiteral(model& m, int literal, long reason, int* pointer, database& d);
    void hardPropagate(model& m);

    bool isSatisfied(model& m, int literal);
    bool isFalsified(model& m, int literal);
    bool isUnassigned(model& m, int literal);
    bool findNextNonFalsified(model& m, int*& ptr, int*& current, int& lit);
}

//
// namespace Assignment {
// 	extern int* stack;			//Points to the first literal in the stack.
// 	extern int* stackCurrent;	//Points to the first unprocessed literal.
// 	extern int* stackUsed;		//Points to after the last literal in the stack.
//
// 	extern int* savedCurrent;
// 	extern int* savedUsed;
// 	extern bool savedConflict;
//
// 	extern bool* falsified;
// 	extern int** stackPointers;
// 	extern bool conflict;
//
// 	extern int watchIterator_;		//processStackSoft processStackHard
// 	extern int literal_;				//unassignSoftLiteral findReason unpropagateClause
// 	extern int* stackLeft_;				//unpropagateClause
// 	extern int* stackRight_;			//unpropagateClause findReason
// 	extern bool unpropagate_;			//unpropagateClause
//
// 	void initialize();
// 	void deallocate();
//
// 	void assignHardLiteral(int lit, long offset);
// 	void assignSoftLiteral(int lit, long offset);
// 	void unassignHardLiteral(int lit);
// 	void unassignSoftLiteral(int lit);
// 	void assignAssumedLiteral(int lit);
//
// 	bool propagateHard();
// 	bool propagateSoft();
//
// 	bool processStackHard();
// 	bool processStackSoft();
//
// 	void force();
// 	void restore();
//
// 	// void print();
// }

#endif
