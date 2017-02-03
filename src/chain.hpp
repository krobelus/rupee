#ifndef __CHAIN_H_INCLUDED__
#define __CHAIN_H_INCLUDED__

namespace Chain {
	extern long* reasons;
	extern long* chain;
	extern long* chainUsed;
	extern bool* shifts;
	extern int shiftSize;
	extern int* shiftQueue;
	extern int* shiftUsed;
	extern int* discardedQueue;
	extern int* discardedUsed;

	extern int* stackIterator_;
	extern int* clause_;
	extern int pivot_;
	extern int literal_;
	extern long offset_;
	extern long* chainIterator_;
	extern int* ptrA_;
	extern int* ptrB_;

	void initialize();
	void deallocate();

	int* findReason(long offset);

	void cleanShift();
	void addResolution(int pivot);
	void generateChain();
	void markChain();
}

#endif
