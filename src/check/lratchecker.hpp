#ifndef __LRATCHECKER_H_INCLUDED__
#define __LRATCHECKER_H_INCLUDED__

#include "hashtable.hpp"
#include "proof.hpp"
#include "database.hpp"

//************************************************************
// Class LratChecker
//************************************************************

class LratChecker {
//------------------------------------------------------------
// Data members
private:
	ClauseDatabase& m_clausedb;
	ChainDatabase& m_chaindb;
	ResolventDatabase& m_resolventdb;
	ChainDatabase& m_deletiondb;
	LratProof& m_proof;
	ClauseArray m_array;
	bool m_empty;

//------------------------------------------------------------
// Builders
public:
	LratChecker(ClauseDatabase& clausedb, ChainDatabase& chaindb,
				ResolventDatabase& resolventdb, ChainDatabase& deletiondb,
				LratProof& proof);

//------------------------------------------------------------
// Interface methods
public:
	void check();
	
//------------------------------------------------------------
// Internal methods
private:
	void propagateChain(ChainIterator& it);
	void checkRupInference(Long clause, Long chain);
	void checkRatInference(Long clause, Long chain, Long resolvents);
	void processDeletions(Long chain);
	void activateClause(Long clause);
	void deactivateClause(Long clause);
};

#endif