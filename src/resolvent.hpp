#ifndef __RESOLVENT_H_INCLUDED__
#define __RESOLVENT_H_INCLUDED__

namespace Resolvent {
	extern long* candidatesQueue;
	extern int candidatesUsed;
	extern int candidatesMax;

	extern int* resolventClause;
	extern int* resolventOriginal;
	extern int* resolventUsed;
	extern bool* resolventLiterals;
	extern int pivot;

	extern long offset_;
	extern int literalIterator_;
	extern long* watchIterator_;
	extern int* clause_;
	extern int readLiteral_;
	extern bool done_;
	extern int* resolventIterator_;

	void initialize();
	void reallocate();
	void deallocate();

	bool containsLiteral(int* &clause,  int literal);
	void addCandidate(long offset);
	void findResolutionCandidates(int literal);

	void setupResolutionSystem(int* clause, int pivot);
	bool computeNextResolvent(long* &candidateIterator);
	void cleanResolvent();
}

#endif
