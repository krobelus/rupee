#ifndef __CORE_H_INCLUDED__
#define __CORE_H_INCLUDED__

#include <string>

//************************************************************
// Static class Core
//************************************************************

class Core {
//------------------------------------------------------------
// Static data members
private:
	static int m_state;

//------------------------------------------------------------
// Static interface methods
public:
	static void accept(const std::string& msg);
	static void reject(const std::string& msg);
	static void invalid(const std::string& msg);
	static void fail(const std::string& msg);
	static void bug(const std::string& msg);
	static void warning(const std::string& msg);
	static bool undecided();
	static void print();

//------------------------------------------------------------
// Constant definitions
	static constexpr int Undecided = 0;
	static constexpr int Accept = 1;
	static constexpr int Reject = 2;
	static constexpr int Invalid = 3;
	static constexpr int Fail = 4;
	static constexpr int Bug = 5;
};

using Short = std::uint32_t;
using Long = std::uint64_t;

//************************************************************
// Static class Shortie
//************************************************************

class Shortie {
//------------------------------------------------------------
// Static interface methods
public:
	static Short toShort(int number);
	static Short complement(const Short& x);
	static Short bound(std::uint32_t number);

	static bool split(Long& value, Short& truncation);
	static bool conflate(Short part, Long& value);

	static std::string toString(Short x);

	static int compare(const void* x1, const void* x2);

//------------------------------------------------------------
// Constant declarations
public:
	static constexpr Short SignMask = 0b1U;
	static constexpr Short AbsoluteMask = ~SignMask;
	static constexpr Short PositiveZero = 0b0U;
	static constexpr Short NegativeZero = 0b1U;
	static constexpr Long ShortShift = 31UL;
	static constexpr Long LongMask = (~0UL) << ShortShift;
	static constexpr Long ShortMask = ~LongMask;
};

#endif