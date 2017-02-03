#ifndef __ASSIGNMENT_H_INCLUDED__
#define __ASSIGNMENT_H_INCLUDED__

#include <stdlib.h>

namespace Assignment {
	extern int* stack;			//Points to the first literal in the stack.
	extern int* stackCurrent;	//Points to the first unprocessed literal.
	extern int* stackUsed;		//Points to after the last literal in the stack.

	extern int* savedCurrent;
	extern int* savedUsed;
	extern bool savedConflict;

	extern bool* falsified;
	extern int** stackPointers;
	extern bool conflict;

	extern int watchIterator_;		//processStackSoft processStackHard
	extern int literal_;				//unassignSoftLiteral findReason unpropagateClause
	extern int* stackLeft_;				//unpropagateClause
	extern int* stackRight_;			//unpropagateClause findReason
	extern bool unpropagate_;			//unpropagateClause

	void initialize();
	void deallocate();

	void assignHardLiteral(int lit, long offset);
	void assignSoftLiteral(int lit, long offset);
	void unassignHardLiteral(int lit);
	void unassignSoftLiteral(int lit);
	void assignAssumedLiteral(int lit);

	bool propagateHard();
	bool propagateSoft();

	bool processStackHard();
	bool processStackSoft();

	void force();
	void restore();

	// void print();
}

#endif
