#ifndef __HASHTABLE_H_INCLUDED__
#define __HASHTABLE_H_INCLUDED__

using Hash = std::uint32_t;

class ClauseDatabase;
class ClauseIterator;
class HashPair;
class HashVector;
class HashTable;

//************************************************************
// Class ClauseIterator
//************************************************************

class HashPair {
//------------------------------------------------------------
// Data members
private:
	Hash m_subhash;
	Long m_clause;

//------------------------------------------------------------
// Builders
public:
	HashPair(ClauseIterator& it);
	HashPair(Hash subhash, Long clause);

//------------------------------------------------------------
// Interface methods
public:
	Hash subhash();
	Long clause();
};


//************************************************************
// Class HashRow
//************************************************************

class HashRow {
//------------------------------------------------------------
// Data members
private:
	HashPair* m_array;
	std::uint32_t m_size;
	std::uint32_t m_capacity;

//------------------------------------------------------------
// Builders
public:
	HashRow(std::uint32_t capacity);
	~HashRow();

//------------------------------------------------------------
// Interface methods
public:
	bool insert(ClauseIterator& it, Long& run);
	bool remove(ClauseIterator& it, Long& run);
	void force(ClauseIterator& it);
	bool find(ClauseIterator& it, Hash subhash, Long& found);

//------------------------------------------------------------
// Internal methods
private:
	bool find(ClauseIterator& it, Hash subhash, std::uint32_t& position);
	void force(Hash subhash, Long clause);
	void drop(std::uint32_t position);
	bool full();
	void reallocate();

//------------------------------------------------------------
// Constant definitions
public:
	static constexpr std::uint32_t ReallocationFactor = 2U;
};


//************************************************************
// Class HashTable
//************************************************************

class HashTable {
//------------------------------------------------------------
// Data members
private:
	HashRow* m_table;

//------------------------------------------------------------
// Builders
public:
	HashTable(std::uint32_t width);
	~HashTable();

//------------------------------------------------------------
// Interface methods
public:
	bool insert(ClauseIterator& it, Long& run);
	bool remove(ClauseIterator& it, Long& run);
	void force(ClauseIterator& it);
	bool find(ClauseIterator& it, Long& run);

//------------------------------------------------------------
// Static methods
public:
	static Hash MainHash(ClauseIterator& it);
	static Hash SubHash(ClauseIterator& it);

//------------------------------------------------------------
// Constant definitions
public:
	static constexpr std::uint32_t Length = 1000000U;
	static constexpr Hash FnvOrigin = 2166136261U;
	static constexpr Hash FnvStep = 16777619U;
};

#endif