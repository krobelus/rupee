#ifndef __PROOF_H_INCLUDED__
#define __PROOF_H_INCLUDED__

namespace Proof {
	extern long* proof;
	extern int max;
	extern int used;

	void allocate();
	void reallocate();
	void deallocate();

	void storeInstruction(long clausepos, int kind);
	void getInstruction(long* instruction, long &offset, int &kind);

	void print();
}

#endif
