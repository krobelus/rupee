#ifndef __LEXER_H_INCLUDED__
#define __LEXER_H_INCLUDED__

#include <fstream>
#include <string>


class Lexer {
public:
    enum class State {
        Closed = 0,
        Unbuffered = 1,
        Buffered = 2,
        Ready = 3,
        Done = 4
    };

private:
    std::ifstream m_input;
    std::string m_buffer;
    std::string::iterator m_iterator;
    State m_state;

public:
    Lexer();

private:
    void openfile(const std::string& path);
    void lineskip();
    void whiteskip();
    void wordskip();

public:
    bool closed();
    void open(const std::string& path);
    void close();
    void next();
    bool end();
    std::string read();
};

#endif