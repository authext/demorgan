#ifndef PARSER_HPP
#define PARSER_HPP

#include <optional>

#include "expression.hpp"
#include "lexer.hpp"

class parser
{
public:
    explicit parser(lexer& lex);

    std::unique_ptr<expression> parse_expression();

private:
    lexer& lex_;

    std::unique_ptr<token> current_token;

    void next();

    std::unique_ptr<expression> parse_add_expression();
    std::unique_ptr<expression> parse_mul_expression();
    std::unique_ptr<expression> parse_unary_expression();
    std::unique_ptr<expression> parse_primary_expression();
    std::unique_ptr<expression> parse_identifier_expression();
};

#endif
