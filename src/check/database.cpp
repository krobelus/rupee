#include <cstdlib>
#include <iostream>

#include "core.hpp"
#include "database.hpp"

//************************************************************
// Class BalancedSet
//************************************************************
// Builders

BalancedSet::BalancedSet(std::uint32_t capacity) :
	m_array(nullptr),
	m_capacity(Shortie::bound(capacity))
{
	m_array = static_cast<bool*>(std::malloc(m_capacity * sizeof(bool)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
	for(Short literal = Shortie::PositiveZero; literal < m_capacity; literal++) {
		m_array[literal] = false;
	}
}

BalancedSet::~BalancedSet() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}

//------------------------------------------------------------
// Interface methods

bool BalancedSet::insert(Short literal) {
	if(literal >= m_capacity) {
		reallocate(literal);
	}
	if(!m_array[literal]) {
		m_array[literal] = true;
		if(m_array[Shortie::complement(literal)]) {
			m_array[TautologyLiteral] = true;
		}
		return true;
	}
	return false;
}

void BalancedSet::reset(ClauseIterator& it) {
	while(!it.end()) {
		m_array[it.get()] = false;
		it.next();
	}
	m_array[TautologyLiteral] = false;
}

void BalancedSet::reset(Short* array, std::uint32_t size) {
	for(std::uint32_t i = 0; i < size; ++i) {
		m_array[array[i]] = false;
	}
	m_array[TautologyLiteral] = false;
}

bool BalancedSet::tautology() {
	return m_array[TautologyLiteral];
}

bool BalancedSet::contains(Short literal) {
	return m_array[literal];
}

//------------------------------------------------------------
// Internal methods

void BalancedSet::reallocate(Short literal) {
	Short newcapacity = m_capacity << 1;
	m_array = static_cast<bool*>(std::realloc(m_array, newcapacity * sizeof(bool)));
	if(m_array == nullptr) {
		Core::fail("Reallocation error");
	}
	for(Short literal = m_capacity; literal < newcapacity; literal++) {
		m_array[literal] = false;
	}
	m_capacity = newcapacity;
}


//************************************************************
// Class ShortDatabase
//************************************************************
// Builders

ShortDatabase::ShortDatabase(Long capacity) :
	m_array(nullptr),
	m_capacity(capacity)
{
	m_array = static_cast<Short*>(std::malloc(capacity * sizeof(Short)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
	m_size = 0;
	close();
}

ShortDatabase::~ShortDatabase() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}


//------------------------------------------------------------
// Interface methods

void ShortDatabase::insert(Short value) {
	if(full()) {
		reallocate();
	}
	m_array[m_size++] = value;
}

void ShortDatabase::setFlag(Long run, Short mask, Short flag) {
	(m_array[run - FlagsCell] &= ~mask) |= flag;
}

Short ShortDatabase::getFlag(Long run, Short mask) {
	return m_array[run - FlagsCell] & mask;
}

void ShortDatabase::reset() {
	rewind(m_run);
}

Long ShortDatabase::close() {
	insert(Separator);
	if(full()) {
		reallocate();
	}
	Long ans = m_run;
	m_run = (m_size += FlagsCell);
	return ans;
}

void ShortDatabase::rewind(Long run) {
	m_size = run;
}

void ShortDatabase::sort() {
	std::qsort(m_array + m_run, m_size - m_run, sizeof(Short), Shortie::compare);
}

//------------------------------------------------------------
// Internal methods

void ShortDatabase::reallocate() {
	m_capacity *= ReallocationFactor;
	m_array = static_cast<Short*>(std::realloc(m_array, m_capacity * sizeof(Short)));
	if(m_array == nullptr) {
		Core::fail("Reallocation error");
	}
}

bool ShortDatabase::full() {
	return m_size >= m_capacity;
}


//************************************************************
// Class specialization ClauseDatabase
//************************************************************
// Builders

ClauseDatabase::ClauseDatabase(Long capacity, std::uint32_t variables) :
	ShortDatabase(capacity),
	m_balanced(variables)
{}

//------------------------------------------------------------
// Interface methods

FormulaIterator ClauseDatabase::getIterator() {
	return FormulaIterator(m_array, m_size, Shortie::PositiveZero);
}

FormulaIterator ClauseDatabase::getIterator(Short literal) {
	return FormulaIterator(m_array, m_size, literal);
}

ClauseIterator ClauseDatabase::getClause(Long run) {
	return ClauseIterator(m_array, run);
}

void ClauseDatabase::insert(Short value) {
	if(m_balanced.insert(value)) {
		ShortDatabase::insert(value);
	}
}

bool ClauseDatabase::tautology() {
	return m_balanced.tautology();
}

void ClauseDatabase::reset() {
	rewind(m_run);
}

Long ClauseDatabase::close() {
	Long oldrun = ShortDatabase::close();
	ClauseIterator it(m_array, oldrun);
	m_balanced.reset(it);
	return oldrun;
}

void ClauseDatabase::rewind(Long run) {
	ShortDatabase::insert(Separator);
	ClauseIterator it(m_array, m_run);
	m_balanced.reset(it);
	m_size = run;
	m_run = run;
}

void ClauseDatabase::setActivity(Long run, bool value) {
	setFlag(run, ActivityMask, value ? ActiveFlag : InactiveFlag);
}

bool ClauseDatabase::active(Long run) {
	return getFlag(run, ActivityMask) == ActiveFlag;
}

Short ClauseDatabase::pivot(Long run) {
	return m_array[run];
}

bool ClauseDatabase::empty(Long run) {
	return m_array[run] == ShortDatabase::Separator;
}

//************************************************************
// Class FormulaIterator
//************************************************************
// Builders

FormulaIterator::FormulaIterator(Short* array, Long size, Short literal) :
	m_array(array),
	m_size(size),
	m_run(2UL),
	m_current(2UL),
	m_literal(literal)
{
	if(literal != Shortie::PositiveZero) {
		nextLiteral();
	} else {
		next();
	}
}

//------------------------------------------------------------
// Interface methods

void FormulaIterator::next() {
	Short read;
	while(m_current < m_size) {
		read = m_array[m_current++];
		if(read == ShortDatabase::Separator) {
			++m_current;
			if((m_array[m_run - ShortDatabase::FlagsCell] & ClauseDatabase::ActivityMask) == ClauseDatabase::InactiveFlag) {
				m_run = m_current;
			} else {
				return;
			}
		}
	}
}

void FormulaIterator::nextLiteral() {
	Short read;
	bool found = false;
	while(m_current < m_size) {
		read = m_array[m_current++];
		if(read == ShortDatabase::Separator) {
			++m_current;
			if(!found || (m_array[m_run - ShortDatabase::FlagsCell] & ClauseDatabase::ActivityMask) == ClauseDatabase::InactiveFlag) {
				m_run = m_current;
				found = false;
			} else {
				return;
			}
		} else if(read == m_literal) {
			found = true;
		}
	}
}

void FormulaIterator::reset() {
	m_current = 0L;
}

Long FormulaIterator::get() {
	return m_run;
}

bool FormulaIterator::end() {
	return m_current >= m_size;
}

//************************************************************
// Class ClauseIterator
//************************************************************
// Builders

ClauseIterator::ClauseIterator(Short* array, Long run) :
	m_array(array),
	m_run(run)
{
	rewind();
}

ClauseIterator::ClauseIterator(ClauseIterator& it, Long run) :
	m_array(it.m_array),
	m_run(run)
{
	rewind();
}

//------------------------------------------------------------
// Interface methods

Short ClauseIterator::get() {
	return m_value;
}

Long ClauseIterator::run() {
	return m_run;
}

void ClauseIterator::next() {
	m_value = m_array[m_offset++];
}

bool ClauseIterator::end() {
	return m_value == ShortDatabase::Separator;
}

void ClauseIterator::rewind() {
	m_offset = m_run;
	m_value = m_array[m_offset++];
}


bool ClauseIterator::equals(ClauseIterator& that) {
	while(!that.end() && !end()) {
		if(that.get() != get()) {
			that.rewind();
			rewind();
			return false;
		}
		that.next();
		next();
	}
	bool ans = (that.end() && end());
	rewind();
	that.rewind();
	return ans;
}

bool ClauseIterator::contains(Short literal) {
	while(!end()) {
		if(get() == literal) {
			return true;
		}
		next();
	}
	return false;
}


//************************************************************
// Class specialization ChainDatabase
//************************************************************
// Builders

ChainDatabase::ChainDatabase(Long capacity) :
	ShortDatabase(capacity)
{}

//------------------------------------------------------------
// Interface methods

void ChainDatabase::insert(Long value) {
	if(almostFull()) {
		reallocate();
	}
	Short truncation;
	while(Shortie::split(value, truncation)) {
		m_array[m_size++] = truncation;
	}
	m_array[m_size++] = truncation;
}

ChainIterator ChainDatabase::getChain(Long run) {
	return ChainIterator(m_array, run);
}

//------------------------------------------------------------
// Internal methods

bool ChainDatabase::almostFull() {
	return m_size + 4 >= m_capacity;
}


//************************************************************
// Class ChainIterator
//************************************************************
// Builders

ChainIterator::ChainIterator(Short* array, Long run) :
	m_array(array),
	m_run(run)
{
	rewind();
}

//------------------------------------------------------------
// Interface methods

Long ChainIterator::get() {
	return m_value;
}

void ChainIterator::next() {
	Short part = m_array[m_offset++];
	m_value = 0UL;
	if(part != ShortDatabase::Separator) {
		while(Shortie::conflate(part, m_value)) {
			part = m_array[m_offset++];
		}
	}
}

bool ChainIterator::end() {
	return m_value == 0UL;
}

void ChainIterator::rewind() {
	m_offset = m_run;
	next();
}


//************************************************************
// Class ResolventDatabase
//************************************************************
// Builders

ResolventDatabase::ResolventDatabase(Long capacity) :
	m_array(nullptr),
	m_size(0),
	m_capacity(capacity)
{
	m_array = static_cast<Long*>(std::malloc(capacity * sizeof(Long)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
	close();
}

ResolventDatabase::~ResolventDatabase() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}

//------------------------------------------------------------
// Interface methods

void ResolventDatabase::insert(Long clause, Long chain) {
	if(full()) {
		reallocate();
	}
	m_array[m_size++] = clause;
	m_array[m_size++] = chain;
}

ResolventIterator ResolventDatabase::getResolvent(Long run) {
	return ResolventIterator(m_array, run);
}

Long ResolventDatabase::close() {
	if(full()) {
		reallocate();
	}
	m_array[m_size++] = Separator;
	Long ans = m_run;
	m_run = m_size;
	return ans;
}

//------------------------------------------------------------
// Internal methods

void ResolventDatabase::reallocate() {
	m_capacity *= ReallocationFactor;
	m_array = static_cast<Long*>(std::realloc(m_array, m_capacity * sizeof(Long)));
}

bool ResolventDatabase::full() {
	return m_size + 1 >= m_capacity;
}


//************************************************************
// Class ResolventIterator
//************************************************************
// Builders

ResolventIterator::ResolventIterator(Long* array, Long run) :
	m_array(array),
	m_run(run)
{}

//------------------------------------------------------------
// Interface methods

bool ResolventIterator::find(Long clause) {
	Long offset = m_run;
	Long cls;
	while((cls = m_array[offset]) != ResolventDatabase::Separator) {
		if(cls == clause) {
			m_chain = m_array[offset + 1];
			return true;
		} else {
			offset += 2;
		}
	}
	return false;
}

Long ResolventIterator::get() {
	return m_chain;
}
