#ifndef __SICKCHECKER_H_INCLUDED__
#define __SICKCHECKER_H_INCLUDED__

#include "hashtable.hpp"
#include "proof.hpp"
#include "database.hpp"

//************************************************************
// Class SickChecker
//************************************************************

class SickChecker {
//------------------------------------------------------------
// Data members
private:
	ClauseDatabase& m_clausedb;
	DratProof& m_proof;
	SickInstruction& m_sick;
	ClauseArray m_array;
	Long m_resolvent;

//------------------------------------------------------------
// Builders
public:
	SickChecker(ClauseDatabase& clausedb, DratProof& proof,
				SickInstruction& sick);

//------------------------------------------------------------
// Interface methods
public:
	void instructionCheck();
	void naturalCheck();
	void resolventCheck();

//------------------------------------------------------------
// Internal methods
private:
	bool upModelCheck();

//------------------------------------------------------------
// Constant definitions
public:
	static constexpr Short ZeroLiteral = Shortie::PositiveZero;
};

#endif