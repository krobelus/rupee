#ifndef __TRAIL_H_INCLUDED__
#define __TRAIL_H_INCLUDED__

#include <stdlib.h>

struct stack {
    int* array;
    int* current;
    int* used;
    bool conflict;
};

struct savestack {
    int* current;
    int* used;
    bool conflict;
};

struct assignment {
    bool* satisfied;
    int** trailpositions;
    long* reasons;
};

struct trail {
    stack Stack;
    savestack SaveStack;
    assignment Assignment;
};

namespace Trail {
    bool allocate(trail& t);
    void deallocate(trail& t);
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