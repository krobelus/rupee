#ifndef __PROOF_H_INCLUDED__
#define __PROOF_H_INCLUDED__

#include "core.hpp"
#include "database.hpp"

//************************************************************
// Class ClauseArray
//************************************************************

class ClauseArray {
//------------------------------------------------------------
// Data members
private:
	Short* m_array;
	std::uint32_t m_size;
	std::uint32_t m_capacity;
	BalancedSet m_balanced;

//------------------------------------------------------------
// Builders
public:
	ClauseArray(std::uint32_t capacity);
	~ClauseArray();

//------------------------------------------------------------
// Interface methods
public:
	void insertClause(ClauseIterator& it);
	void insertResolvent(ClauseIterator& it1, ClauseIterator& it2, Short pivot);
	void reverseClause(ClauseIterator& it);
	void reverseResolvent(ClauseIterator& it1, ClauseIterator& it2, Short pivot);
	void propagateClause(ClauseIterator& it);
	bool containsReversed(ClauseIterator& it);
	bool containsReversedResolvent(ClauseIterator& it1, ClauseIterator& it2, Short pivot);
	bool upModels(ClauseIterator& it);
	bool tautology();
	void reset();

//------------------------------------------------------------
// Internal methods
private:
	bool contains(Short literal);
	void insert(Short literal);
	bool fits(Short literal);
	bool full();
	void reallocate();

//------------------------------------------------------------
// Constant definitions
public:
	static constexpr std::uint32_t ReallocationFactor = 2U;
	static constexpr Short Falsified = Shortie::NegativeZero;
	static constexpr Short NoPropagation = Shortie::PositiveZero;
};



//************************************************************
// Class LratInference
//************************************************************

class LratInference {
//------------------------------------------------------------
// Data members
private:
	std::int32_t m_kind;
	Long m_clause;
	Long m_chain;
	Long m_resolvents;
	Long m_deletion;

//------------------------------------------------------------
// Builders
public:
	LratInference(Long clause);
	LratInference(std::int32_t kind, Long clause, Long chain);

//------------------------------------------------------------
// Interface methods
public:
	void addResolvents(Long resolvents);
	void addDeletion(Long deletion);
	bool isPremise();
	bool isRup();
	bool isRat();
	Long getClause();
	Long getChain();
	Long getResolvents();
	Long getDeletion();

//------------------------------------------------------------
// Constant declarations
public:
	static constexpr std::int32_t NoRun = 0;
	static constexpr std::int32_t PremiseKind = 1;
	static constexpr std::int32_t RupKind = 2;
	static constexpr std::int32_t RatKind = 3;
};


//************************************************************
// Class LratProof
//************************************************************

class LratProof {
//------------------------------------------------------------
// Data members
private:
	LratInference* m_array;
	std::uint32_t m_size;
	std::uint32_t m_capacity;
	std::uint32_t m_iterator;

//------------------------------------------------------------
// Builders
public:
	LratProof(std::uint32_t capacity);
	~LratProof();

//------------------------------------------------------------
// Interface methods
public:
	std::uint32_t insertPremise(Long clause);
	std::uint32_t insertRup(Long clause, Long chain);
	std::uint32_t insertRat(Long clause, Long chain);
	void addResolventChain(std::uint32_t inference, Long chain);
	void addDeletionChain(std::uint32_t inference, Long chain);
	bool end();
	void next();
	LratInference& get();
	// void print(ClauseDatabase& clausedb,
	// 		ChainDatabase& chaindb, ResolventDatabase& resolventdb,
	// 		ChainDatabase& deletiondb, ClauseDictionary& dictionary);

//------------------------------------------------------------
// Private methods
private:
	void insert(const LratInference& inf);
	void reallocate();
	bool full();

//------------------------------------------------------------
// Constant definitions
	static constexpr std::uint32_t ReallocationFactor = 2;
	static constexpr Long NoRun = 0UL;
};



//************************************************************
// Class DratInference
//************************************************************

class DratInference {
//------------------------------------------------------------
// Data members
private:
	std::int32_t m_kind;
	Long m_clause;
	Short m_pivot;

//------------------------------------------------------------
// Builders
public:
	DratInference(std::int32_t kind, Long clause, Short pivot);

//------------------------------------------------------------
// Interface methods
public:
	bool isPremise();
	bool isIntroduction();
	bool isDeletion();
	Long getClause();
	Short getPivot();

//------------------------------------------------------------
// Constant declarations
public:
	static constexpr std::int32_t PremiseKind = 1;
	static constexpr std::int32_t IntroductionKind = 2;
	static constexpr std::int32_t DeletionKind = 3;
};


//************************************************************
// Class DratProof
//************************************************************

class DratProof {
//------------------------------------------------------------
// Data members
private:
	DratInference* m_array;
	std::uint32_t m_size;
	std::uint32_t m_capacity;

//------------------------------------------------------------
// Builders
public:
	DratProof(std::uint32_t capacity);
	~DratProof();

//------------------------------------------------------------
// Interface methods
public:
	std::uint32_t insertPremise(Long clause);
	std::uint32_t insertIntroduction(Long clause, Short pivot);
	std::uint32_t insertDeletion(Long clause);
	void accumulateFormula(std::uint32_t index, ClauseDatabase& db);
	DratInference getInstruction(std::uint32_t index);

//------------------------------------------------------------
// Private methods
private:
	void insert(const DratInference& inf);
	void reallocate();
	bool full();

//------------------------------------------------------------
// Constant definitions
	static constexpr std::uint32_t ReallocationFactor = 2;
};


//************************************************************
// Class SickInstruction
//************************************************************

class SickInstruction {
//------------------------------------------------------------
// Data members
private:
	Short m_pivot;
	std::uint32_t m_instruction;
	Long m_natclause;
	Long m_natmodel;
	Long m_resclause;
	Long m_resmodel;

//------------------------------------------------------------
// Interface methods
public:
	Short& pivot();
	std::uint32_t& instruction();
	Long& naturalClause();
	Long& naturalModel();
	Long& resolventClause();
	Long& resolventModel();
};


#endif