#ifndef LEXER_HPP
#define LEXER_HPP

#include <iosfwd>
#include <map>
#include <optional>

#include "position.hpp"
#include "token.hpp"

class lexer
{
public:
    enum class table_state
    {
        REJECT,
        ACCEPT,
        CONTINUE,
    };

    enum class accept_state
    {
        NONE,
        IDENTIFIER,
        OPERATOR,
        WHITESPACE,
    };

    struct state
    {
        table_state tbl;
        accept_state acc;
    };

    explicit lexer(std::istream& stream);

    std::unique_ptr<token> next_token();

private:
    std::istream& stream_;
    position current_pos;

    char prev_char;

    char read();
};

#endif
