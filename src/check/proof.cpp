#include <cstdlib>
#include <iostream>

#include "core.hpp"
#include "proof.hpp"

#include "database.hpp"

//************************************************************
// Class ClauseArray
//************************************************************
// Builders

ClauseArray::ClauseArray(std::uint32_t capacity) :
	m_array(nullptr),
	m_size(0U),
	m_capacity(capacity),
	m_balanced(capacity)
{
	m_array = static_cast<Short*>(std::malloc(m_capacity * sizeof(Short)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
}

ClauseArray::~ClauseArray() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}

//------------------------------------------------------------
// Interface methods

void ClauseArray::insertClause(ClauseIterator& it) {
	while(!it.end()) {
		insert(it.get());
		it.next();
	}
}

void ClauseArray::insertResolvent(ClauseIterator& it1, ClauseIterator& it2, Short pivot) {
	Short literal;
	while(!it1.end()) {
		if((literal = it1.get()) != pivot) {
			insert(literal);
		}
		it1.next();
	}
	pivot = Shortie::complement(pivot);
	while(!it2.end()) {
		if((literal = it2.get()) != pivot) {
			insert(literal);
		}
		it2.next();
	}
}

void ClauseArray::reverseClause(ClauseIterator& it) {
	while(!it.end()) {
		insert(Shortie::complement(it.get()));
		it.next();
	}
}

void ClauseArray::reverseResolvent(ClauseIterator& it1, ClauseIterator& it2, Short pivot) {
	Short literal;
	while(!it1.end()) {
		if((literal = it1.get()) != pivot) {
			insert(Shortie::complement(literal));
		}
		it1.next();
	}
	pivot = Shortie::complement(pivot);
	while(!it2.end()) {
		if((literal = it2.get()) != pivot) {
			insert(Shortie::complement(literal));
		}
		it2.next();
	}
}

void ClauseArray::propagateClause(ClauseIterator& it) {
	Short literal = Falsified;
	Short propagation = Falsified;
	while(!it.end()) {
		literal = it.get();
		if(!m_balanced.contains(Shortie::complement(literal))) {
			if(propagation != Falsified) {
				return;
			}
			propagation = literal;
		}
		it.next();
	}
	if(propagation == Falsified) {
		propagation = literal;
	}
	if(propagation != Falsified) {
		insert(propagation);
	}
}

bool ClauseArray::containsReversed(ClauseIterator& it) {
	while(!it.end()) {
		if(!contains(Shortie::complement(it.get()))) {
			return false;
		}
		it.next();
	}
	return true;
}

bool ClauseArray::containsReversedResolvent(ClauseIterator& it1, ClauseIterator& it2, Short pivot) {
	Short literal;
	while(!it1.end()) {
		if((literal = it1.get()) != pivot) {
			if(!contains(Shortie::complement(it1.get()))) {
				return false;
			}
		}
		it1.next();
	}
	pivot = Shortie::complement(pivot);
	while(!it2.end()) {
		if((literal = it2.get()) != pivot) {
			if(!contains(Shortie::complement(it2.get()))) {
				return false;
			}
		}
		it2.next();
	}
	return true;
}

bool ClauseArray::upModels(ClauseIterator& it) {
	Short found = Falsified;
	Short literal;
	while(!it.end()) {
		literal = it.get();
		if(!contains(Shortie::complement(literal))) {
			if(contains(literal)) {
				return true;
			} else if(found == Falsified) {
				found = literal;
			} else {
				return true;
			}
		}
		it.next();
	}
	return false;
}

bool ClauseArray::tautology() {
	return m_balanced.tautology();
}

void ClauseArray::reset() {
	m_balanced.reset(m_array, m_size);
	m_size = 0U;
}

//------------------------------------------------------------
// Private methods

bool ClauseArray::contains(Short literal) {
	return fits(literal) && m_balanced.contains(literal);
}

void ClauseArray::insert(Short literal) {
	if(m_balanced.insert(literal)) {
		if(full()) {
			reallocate();
		}
		m_array[m_size++] = literal;
	}
}

bool ClauseArray::full() {
	return m_size >= m_capacity;
}

bool ClauseArray::fits(Short literal) {
	return literal < m_capacity;
}

void ClauseArray::reallocate() {
	m_capacity *= ReallocationFactor;
	m_array = static_cast<Short*>(std::realloc(m_array, m_capacity * sizeof(Short)));
	if(m_array == nullptr) {
		Core::fail("Reallocation error");
	}
}

//************************************************************
// Class LratInference
//************************************************************
// Builders

LratInference::LratInference(Long clause) :
	m_kind(PremiseKind),
	m_clause(clause),
	m_chain(NoRun),
	m_resolvents(NoRun),
	m_deletion(NoRun)
{}

LratInference::LratInference(std::int32_t kind, Long clause, Long chain) :
	m_kind(kind),
	m_clause(clause),
	m_chain(chain),
	m_resolvents(NoRun),
	m_deletion(NoRun)
{}

//------------------------------------------------------------
// Interface methods

void LratInference::addResolvents(Long resolvents) {
	if(m_resolvents == NoRun) {
		m_resolvents = resolvents;
	} else {
		Core::invalid("Duplicated resolvent chain assigned to instruction");
	}
}

void LratInference::addDeletion(Long deletion) {
	if(m_deletion == NoRun) {
		m_deletion = deletion;
	} else {
		Core::invalid("Duplicated deletion chain assigned to instruction");
	}	
}

bool LratInference::isPremise() {
	return m_kind == PremiseKind;
}

bool LratInference::isRup() {
	return m_kind == RupKind;
}

bool LratInference::isRat() {
	return m_kind == RatKind;
}

Long LratInference::getClause() {
	return m_clause;
}

Long LratInference::getChain() {
	return m_chain;
}

Long LratInference::getResolvents() {
	return m_resolvents;
}

Long LratInference::getDeletion() {
	return m_deletion;
}


//************************************************************
// Class LratProof
//************************************************************
// Builders

LratProof::LratProof(std::uint32_t capacity) :
	m_array(nullptr),
	m_size(0U),
	m_capacity(capacity),
	m_iterator(0U)
{
	m_array = static_cast<LratInference*>(std::malloc(m_capacity * sizeof(LratInference)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
}

LratProof::~LratProof() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}

//------------------------------------------------------------
// Interface methods

std::uint32_t LratProof::insertPremise(Long clause) {
	insert(LratInference(clause));
	return m_size - 1;
}

std::uint32_t LratProof::insertRup(Long clause, Long chain) {
	insert(LratInference(LratInference::RupKind, clause, chain));
	return m_size - 1;
}

std::uint32_t LratProof::insertRat(Long clause, Long chain) {
	insert(LratInference(LratInference::RatKind, clause, chain));
	return m_size - 1;
}

void LratProof::addResolventChain(std::uint32_t inference, Long chain) {
	m_array[inference].addResolvents(chain);
}

void LratProof::addDeletionChain(std::uint32_t inference, Long chain) {
	m_array[inference].addDeletion(chain);
}

bool LratProof::end() {
	return m_iterator >= m_size;
}

void LratProof::next() {
	++m_iterator;
}

LratInference& LratProof::get() {
	return m_array[m_iterator];
}

//------------------------------------------------------------
// Private methods

void LratProof::insert(const LratInference& inf) {
	if(full()) {
		reallocate();
	}
	m_array[m_size++] = inf;
}

void LratProof::reallocate() {
	m_capacity *= ReallocationFactor;
	m_array = static_cast<LratInference*>(std::realloc(m_array, m_capacity * sizeof(LratInference)));
	if(m_array == nullptr) {
		Core::fail("Reallocation error");
	}
}

bool LratProof::full() {
	return m_size >= m_capacity;
}


//************************************************************
// Class DratInference
//************************************************************
// Builders

DratInference::DratInference(std::int32_t kind, Long clause, Short pivot) :
	m_kind(kind),
	m_clause(clause),
	m_pivot(pivot)
{}

//------------------------------------------------------------
// Interface methods

bool DratInference::isPremise() {
	return m_kind == PremiseKind;
}

bool DratInference::isIntroduction() {
	return m_kind == IntroductionKind;
}

bool DratInference::isDeletion() {
	return m_kind == DeletionKind;
}

Long DratInference::getClause() {
	return m_clause;
}

Short DratInference::getPivot() {
	return m_pivot;
}

//************************************************************
// Class DratInference
//************************************************************
// Builders

DratProof::DratProof(std::uint32_t capacity) :
	m_array(nullptr),
	m_size(0U),
	m_capacity(capacity)
{
	m_array = static_cast<DratInference*>(std::malloc(m_capacity * sizeof(DratInference)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
}

DratProof::~DratProof() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}

//------------------------------------------------------------
// Interface methods

std::uint32_t DratProof::insertPremise(Long clause) {
	insert(DratInference(DratInference::PremiseKind, clause, Shortie::NegativeZero));
	return m_size - 1;
}

std::uint32_t DratProof::insertIntroduction(Long clause, Short pivot) {
	insert(DratInference(DratInference::IntroductionKind, clause, pivot));
	return m_size - 1;
}

std::uint32_t DratProof::insertDeletion(Long clause) {
	insert(DratInference(DratInference::DeletionKind, clause, Shortie::NegativeZero));
	return m_size - 1;
}

void DratProof::accumulateFormula(std::uint32_t index, ClauseDatabase& db) {
	if(index - 1 >= m_size || index - 1 < 0) {
		Core::invalid("Instruction index in SICK file does not correspond to a DRAT proof instruction");
		return;
	}
	for(std::uint32_t i = 0U; i < index - 1; ++i) {
		DratInference& inference = m_array[i];
		db.setActivity(inference.getClause(), !(inference.isDeletion()));		
	}
}

DratInference DratProof::getInstruction(std::uint32_t index) {
	return m_array[index - 1];
}

//------------------------------------------------------------
// Private methods

void DratProof::insert(const DratInference& inf) {
	if(full()) {
		reallocate();
	}
	m_array[m_size++] = inf;
}

void DratProof::reallocate() {
	m_capacity *= ReallocationFactor;
	m_array = static_cast<DratInference*>(std::realloc(m_array, m_capacity * sizeof(DratInference)));
	if(m_array == nullptr) {
		Core::fail("Reallocation error");
	}
}

bool DratProof::full() {
	return m_size >= m_capacity;
}


//************************************************************
// Class SickInstruction
//************************************************************

Short& SickInstruction::pivot() {
	return m_pivot;
}

std::uint32_t& SickInstruction::instruction() {
	return m_instruction;
}

Long& SickInstruction::naturalClause() {
	return m_natclause;
}

Long& SickInstruction::naturalModel() {
	return m_natmodel;
}

Long& SickInstruction::resolventClause() {
	return m_resclause;
}

Long& SickInstruction::resolventModel() {
	return m_resmodel;
}

