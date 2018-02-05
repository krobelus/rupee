#include <cstdlib>
#include <iostream>

#include "core.hpp"
#include "database.hpp"
#include "hashtable.hpp"

//************************************************************
// Class ClauseIterator
//************************************************************
// Builders

HashPair::HashPair(ClauseIterator& it) :
	m_subhash(HashTable::SubHash(it)),
	m_clause(it.run())
{}

HashPair::HashPair(Hash subhash, Long clause) :
	m_subhash(subhash),
	m_clause(clause)
{}

//------------------------------------------------------------
// Interface methods

Hash HashPair::subhash() {
	return m_subhash;
}

Long HashPair::clause() {
	return m_clause;
}


//************************************************************
// Class HashRow
//************************************************************
// Builders

HashRow::HashRow(std::uint32_t capacity) :
	m_array(nullptr),
	m_size(0U),
	m_capacity(capacity)
{
	m_array = static_cast<HashPair*>(std::malloc(m_capacity * sizeof(HashPair)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
}

HashRow::~HashRow() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}

//------------------------------------------------------------
// Interface methods

bool HashRow::insert(ClauseIterator& it, Long& run) {
	Hash subhash = HashTable::SubHash(it);
	std::uint32_t position;
	if(find(it, subhash, position)) {
		run = m_array[position].clause();
		return true;
	} else {
		run = it.run();
		force(subhash, run);
		return false;
	}
}

bool HashRow::remove(ClauseIterator& it, Long& run) {
	Hash subhash = HashTable::SubHash(it);
	std::uint32_t position;
	if(find(it, subhash, position)) {
		run = m_array[position].clause();
		drop(position);
		return true;
	} else {
		return false;
	}
}

void HashRow::force(ClauseIterator& it) {
	if(full()) {
		reallocate();
	}
	m_array[m_size++] = HashPair(it);
}

bool HashRow::find(ClauseIterator& it, Hash subhash, Long& found) {
	std::uint32_t position;
	bool ans = find(it, subhash, position);
	if(ans) {
		found = m_array[position].clause();
	}
	return ans;
}

//------------------------------------------------------------
// Internal methods

bool HashRow::find(ClauseIterator& it, Hash subhash, std::uint32_t& position) {
	for(std::uint32_t i = 0; i < m_size; ++i) {
		if(m_array[i].subhash() == subhash) {
			ClauseIterator that(it, m_array[i].clause());
			if(it.equals(that)) {
				position = i;
				return true;
			}
		}
	}
	return false;
}


void HashRow::force(Hash subhash, Long clause) {
	if(full()) {
		reallocate();
	}
	m_array[m_size++] = HashPair(subhash, clause);
}

void HashRow::drop(std::uint32_t position) {
	m_array[position] = m_array[--m_size];
}

bool HashRow::full() {
	return m_size >= m_capacity;
}

void HashRow::reallocate() {
	m_capacity *= ReallocationFactor;
	m_array = static_cast<HashPair*>(std::realloc(m_array, m_capacity * sizeof(HashPair)));
	if(m_array == nullptr) {
		Core::fail("Reallocation error");
	}
}


//************************************************************
// Class HashTable
//************************************************************
// Builders

HashTable::HashTable(std::uint32_t width) :
	m_table(nullptr)
{
	m_table = static_cast<HashRow*>(std::malloc(Length * sizeof(HashRow)));
	if(m_table == nullptr) {
		Core::fail("Allocation error");
	}
	for(std::uint32_t i = 0; i < Length; ++i) {
		new(m_table + i) HashRow(width);
	}
}

HashTable::~HashTable() {
	if(m_table != nullptr) {
		for(std::uint32_t i = 0; i < Length; ++i) {
			(m_table + i)->~HashRow();
		}
		std::free(m_table);
	}
}

//------------------------------------------------------------
// Interface methods

bool HashTable::insert(ClauseIterator& it, Long& run) {
	return m_table[MainHash(it)].insert(it, run);
}

bool HashTable::remove(ClauseIterator& it, Long& run) {
	return m_table[MainHash(it)].remove(it, run);
}

void HashTable::force(ClauseIterator& it) {
	m_table[MainHash(it)].force(it);
}

bool HashTable::find(ClauseIterator& it, Long& found) {
	Hash subhash = HashTable::SubHash(it);
	return m_table[MainHash(it)].find(it, subhash, found);
}

//------------------------------------------------------------
// Static methods

Hash HashTable::MainHash(ClauseIterator& it) {
	// Using one-at-a-time hashing by Bob Jenkins
	Hash ans = 0U;
	while(!it.end()) {
		ans += it.get();
		ans += (ans << 10);
		ans ^= (ans >> 6);
		it.next();
	}
	ans += (ans << 3);
	ans ^= (ans >> 11);
	ans += (ans << 15);
	it.rewind();
	return ans % Length;
}

Hash HashTable::SubHash(ClauseIterator& it) {
	// Using FNV hashing
	Hash ans = FnvOrigin;
	while(!it.end()) {
		ans = (ans * FnvStep) ^ it.get();
		it.next();
	}
	it.rewind();
	return ans;
}