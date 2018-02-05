#include <cstdlib>
#include <string>
#include <iostream>

#include "core.hpp"

//************************************************************
// Static class Core
//************************************************************
// Static data members

int Core::m_state = Core::Undecided;

//------------------------------------------------------------
// Static interface methods

void Core::accept(const std::string& str) {
	std::cout << "c " << str << std::endl;
	m_state = Accept;
}

void Core::reject(const std::string& str) {
	std::cout << "c " << str << std::endl;
	m_state = Reject;
}

void Core::fail(const std::string& str) {
    std::cout << "e " << str << std::endl;
    std::cout << "s FAIL" << std::endl;
    m_state = Fail;
    std::exit(1);
}

void Core::bug(const std::string& str) {
    std::cout << "e " << str << std::endl;
    std::cout << "s BUG" << std::endl;
    m_state = Bug;
    std::exit(1);
}

void Core::invalid(const std::string& str) {
    std::cout << "e " << str << std::endl;
    m_state = Invalid;
}

void Core::warning(const std::string& str) {
    std::cout << "w " << str << std::endl;
}

bool Core::undecided() {
	return m_state == Undecided;
}

void Core::print() {
    std::cout << "s ";
    if(m_state == Invalid) {
        std::cout << "INVALID";
    } else if(m_state == Accept) {
        std::cout << "ACCEPTED";
    } else {
        std::cout << "REJECTED";
    }
    std::cout << std::endl;
}


//************************************************************
// Class Shortie
//************************************************************
// Static interface methods

Short Shortie::toShort(int number) {
    return (number >= 0) ?
        (static_cast<std::uint32_t>(number) << 1U) :
        ((static_cast<std::uint32_t>(-number) << 1U) | SignMask);
}

Short Shortie::complement(const Short& x) {
    return x ^ SignMask;
}

Short Shortie::bound(std::uint32_t x) {
    return x << 1U;
}

bool Shortie::split(Long& value, Short& truncation) {
    truncation = static_cast<std::uint32_t>(value & ShortMask) << 1U;
    value >>= ShortShift;
    if(value == 0UL) {
        truncation |= SignMask;
        return false;
    }
    return true;
}

bool Shortie::conflate(Short part, Long& value) {
    (value <<= ShortShift) |= static_cast<std::uint32_t>(part >> 1U);
    return (part & SignMask) == 0U;
}

std::string Shortie::toString(Short x) {
    if((x & SignMask) == 0U) {
        return std::to_string(x >> 1);
    } else {
        return "-" + std::to_string(x >> 1);
    }
}

int Shortie::compare(const void* x1, const void* x2) {
    const Short a1 = *static_cast<const Short*>(x1);
    const Short a2 = *static_cast<const Short*>(x2);
    if(a1 < a2) {
        return -1;
    } else if(a1 > a2) {
        return 1;
    } else {
        return 0;
    }
}