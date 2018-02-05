#ifndef __DATABASE_H_INCLUDED__
#define __DATABASE_H_INCLUDED__

#include "core.hpp"

class BalancedSet;
class ShortDatabase;
class ClauseDatabase;
class ClauseIterator;
class FormulaIterator;
class ChainDatabase;
class ChainIterator;
class ResolventDatabase;
class ResolventIterator;


//************************************************************
// Class BalancedSet
//************************************************************

class BalancedSet {
//------------------------------------------------------------
// Data members
private:
	bool* m_array;
	Short m_capacity;

//------------------------------------------------------------
// Builders
public:
	BalancedSet(std::uint32_t capacity);
	~BalancedSet();

//------------------------------------------------------------
// Interface methods
public:
	bool insert(Short literal);
	void reset(ClauseIterator& it);
	void reset(Short* array, std::uint32_t size);
	bool tautology();
	bool contains(Short literal);

//------------------------------------------------------------
// Internal methods
private:
	void reallocate(Short literal);

//------------------------------------------------------------
// Constant declarations
public:
	static constexpr Short TautologyLiteral = Shortie::PositiveZero;
};

//************************************************************
// Class ShortDatabase
//************************************************************

class ShortDatabase {
//------------------------------------------------------------
// Data members
protected:
	Short* m_array;
	Long m_size;
	Long m_capacity;
	Long m_run;

//------------------------------------------------------------
// Builders
public:
	ShortDatabase(Long capacity);
	~ShortDatabase();

//------------------------------------------------------------
// Interface methods
public:
	void insert(Short value);
	void setFlag(Long run, Short mask, Short flag);
	Short getFlag(Long run, Short mask);
	void reset();
	Long close();
	void rewind(Long run);
	void sort();

//------------------------------------------------------------
// Internal methods
protected:
	void reallocate();
	bool full();

//------------------------------------------------------------
// Constant declarations
public:
	static constexpr Short Separator = Shortie::NegativeZero;
	static constexpr Long NoRun = 0;
	static constexpr Long ReallocationFactor = 2;
	static constexpr Long FlagsCell = 1;
};


//************************************************************
// Class specialization ClauseDatabase
//************************************************************

class ClauseDatabase : public ShortDatabase {
//------------------------------------------------------------
// Additional data members
protected:
	BalancedSet m_balanced;

//------------------------------------------------------------
// Builders
public:
	ClauseDatabase(Long capacity, std::uint32_t variables);

//------------------------------------------------------------
// Interface methods
public:
	FormulaIterator getIterator();
	FormulaIterator getIterator(Short literal);
	ClauseIterator getClause(Long run);
	void insert(Short value);
	bool tautology();
	void reset();
	Long close();
	void rewind(Long run);
	void setActivity(Long run, bool flag);
	bool active(Long run);
	Short pivot(Long run);
	bool empty(Long run);
	// bool next(Long& run, Short literal);
	// bool end(Long& run);

//------------------------------------------------------------
// Constant declarations
public:
	static constexpr Short ActivityMask = 0b1;
	static constexpr Short InactiveFlag = 0b0;
	static constexpr Short ActiveFlag = 0b1;

};

//************************************************************
// Class FormulaIterator
//************************************************************

class FormulaIterator {
//------------------------------------------------------------
// Data members
private:
	Short* m_array;
	Long m_size;
	Long m_run;
	Long m_current;
	Short m_literal;
	
//------------------------------------------------------------
// Builders
public:
	FormulaIterator(Short* array, Long size, Short literal);

//------------------------------------------------------------
// Interface methods
public:
	void next();
	void nextLiteral();
	void reset();
	Long get();
	bool end();
};


//************************************************************
// Class ClauseIterator
//************************************************************

class ClauseIterator {
//------------------------------------------------------------
// Data members
private:
	Short* m_array;
	Long m_run;
	Long m_offset;
	Short m_value;

//------------------------------------------------------------
// Builders
public:
	ClauseIterator(Short* array, Long run);
	ClauseIterator(ClauseIterator& it, Long run);

//------------------------------------------------------------
// Interface methods
public:
	Short get();
	Long run();
	void next();
	bool end();
	void rewind();
	bool equals(ClauseIterator& it);
	bool contains(Short literal);
};


//************************************************************
// Class specialization ChainDatabase
//************************************************************

class ChainDatabase : public ShortDatabase {
//------------------------------------------------------------
// Builders
public:
	ChainDatabase(Long capacity);

//------------------------------------------------------------
// Interface methods
public:
	void insert(Long value);
	ChainIterator getChain(Long run);

//------------------------------------------------------------
// Internal methods
private:
	bool almostFull();
};

//************************************************************
// Class ChainIterator
//************************************************************

class ChainIterator {
//------------------------------------------------------------
// Data members
private:
	Short* m_array;
	Long m_run;
	Long m_offset;
	Long m_value;

//------------------------------------------------------------
// Builders
public:
	ChainIterator(Short* array, Long run);

//------------------------------------------------------------
// Interface methods
public:
	Long get();
	void next();
	bool end();
	void rewind();
};


//************************************************************
// Class specialization ResolventDatabase
//************************************************************

class ResolventDatabase {
//------------------------------------------------------------
// Data members
private:
	Long* m_array;
	Long m_size;
	Long m_capacity;
	Long m_run;

//------------------------------------------------------------
// Builders
public:
	ResolventDatabase(Long capacity);
	~ResolventDatabase();

//------------------------------------------------------------
// Interface methods
public:
	void insert(Long clause, Long chain);
	ResolventIterator getResolvent(Long run);
	Long close();

//------------------------------------------------------------
// Internal methods
protected:
	void reallocate();
	bool full();

//------------------------------------------------------------
// Constant declarations
public:
	static constexpr Long Separator = 0;
	static constexpr Long ReallocationFactor = 2;
};

//************************************************************
// Class ResolventIterator
//************************************************************

class ResolventIterator {
//------------------------------------------------------------
// Data members
private:
	Long* m_array;
	Long m_run;
	Long m_chain;

//------------------------------------------------------------
// Builders
public:
	ResolventIterator(Long* array, Long run);

//------------------------------------------------------------
// Interface methods
public:
	bool find(Long clause);
	Long get();
};


#endif