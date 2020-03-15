#include "parser.hpp"

parser::parser(lexer& lex)
    : lex_(lex)
    , current_token(nullptr)
{
    next();
}

std::unique_ptr<expression> parser::parse_expression()
{
    return parse_add_expression();
}

std::unique_ptr<expression> parser::parse_add_expression()
{
    auto left = parse_mul_expression();
    if (!left)
        return nullptr;

    if (!match_operator_kind(*current_token, token_operator::kind::PIPEPIPE))
        return left;
    next();

    auto right = parse_add_expression();
    if (!right)
        return nullptr;

    return make_binary(expression_binary::kind::OR, std::move(left), std::move(right));
}

std::unique_ptr<expression> parser::parse_mul_expression()
{
    auto left = parse_unary_expression();
    if (!left)
        return nullptr;

    if (!match_operator_kind(*current_token, token_operator::kind::AMPERAMPER))
        return left;
    next();

    auto right = parse_mul_expression();
    if (!right)
        return nullptr;

    return make_binary(expression_binary::kind::AND, std::move(left), std::move(right));
}

std::unique_ptr<expression> parser::parse_unary_expression()
{
    switch (current_token->type())
    {
    case token::type::IDENTIFIER:
        return parse_identifier_expression();

    case token::type::OPERATOR:
    {
        if (!match_operator_kind(*current_token, token_operator::kind::EXCLAM))
            break;
        next();

        auto expr = parse_unary_expression();
        if (!expr)
            return nullptr;

        return make_unary(expression_unary::kind::NOT, std::move(expr));
    }

    default:
        break;
    }

    return parse_primary_expression();
}

std::unique_ptr<expression> parser::parse_primary_expression()
{
    switch (current_token->type())
    {
    case token::type::IDENTIFIER:
        return parse_identifier_expression();

    case token::type::OPERATOR:
    {
        if (!match_operator_kind(*current_token, token_operator::kind::LPAREN))
            return nullptr;
        next();

        auto expr = parse_expression();

        if (!match_operator_kind(*current_token, token_operator::kind::RPAREN))
            return nullptr;
        next();

        return expr;
    }

    default:
        return nullptr;
    }
}

std::unique_ptr<expression> parser::parse_identifier_expression()
{
    switch (current_token->type())
    {
    case token::type::IDENTIFIER:
    {
        const auto tok_ident = dynamic_cast<const token_identifier&>(*current_token);
        next();
        return make_identifier(tok_ident.name());
    }

    default:
        return nullptr;
    }
}

void parser::next()
{
    current_token = lex_.next_token();
}
