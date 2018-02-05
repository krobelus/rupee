#include <cstdlib>
#include <cctype>
#include <iostream>

#include "core.hpp"
#include "lexer.hpp"

Lexer::Lexer() :
	m_state(State::Closed)
{}

void Lexer::openfile(const std::string& path) {
    m_input.open(path, std::ios_base::in);
    if(!m_input.is_open()) {
        Core::fail("Could not open file " + path);
    }
    m_state = State::Unbuffered;
}

void Lexer::lineskip() {
    while(m_input.good()) {
        std::getline(m_input, m_buffer);
        if (!m_buffer.empty() && m_buffer.front() != 'c' && m_buffer.front() != 'p') {
            m_state = State::Buffered;
            m_iterator = m_buffer.begin();
            return;
        }
    }
    if(m_input.eof()) {
        m_state = State::Done;
    } else {
        Core::fail("Input stream failed");
    }
}

void Lexer::whiteskip() {
    while(m_iterator != m_buffer.end()) {
        if(!isspace(*m_iterator)) {
            m_state = State::Ready;
            return;
        }
        ++m_iterator;
    }
    m_state = State::Unbuffered;
}


void Lexer::wordskip() {
    while(m_iterator != m_buffer.end()) {
        if(isspace(*m_iterator)) {
            m_state = State::Buffered;
            return;
        }
        ++m_iterator;
    }
    m_state = State::Unbuffered;
}

bool Lexer::closed() {
    return m_state == State::Closed;
}

void Lexer::open(const std::string& path) {
    openfile(path);
    next();
}

void Lexer::close() {
    m_input.close();
    m_state = State::Closed;
}

void Lexer::next() {
	while(m_state < State::Ready) {
		if(m_state == State::Unbuffered) {
			lineskip();
		} else {
			whiteskip();
		}
	}
}

bool Lexer::end() {
	return m_state == State::Done;
}

std::string Lexer::read() {
	std::string::iterator prev = m_iterator;
	wordskip();
    return std::string(prev, m_iterator);
}