#ifndef __SOLVER_H_INCLUDED__
#define __SOLVER_H_INCLUDED__

namespace Solver {
	extern long* proofIterator;
	extern int proofStatus;

	extern long offset_;
	extern int deletion_;
	extern int* clause_;
	extern int literal_;
	extern bool answer_;
	extern int pivot_;
	extern long* candidateIterator_;
	extern long candidateOffset_;

	void deallocate();

	bool checkProof();
	bool checkClause(long offset);

	bool checkRup(int* clause);
	bool checkRat(int* clause);

	void setStatus(int status);
}

#endif
