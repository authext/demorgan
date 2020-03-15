#include "token.hpp"

#include <utility>

#include <fmt/format.h>

token::token(location loc) noexcept
    : loc_(loc)
{
}

const location& token::loc() const noexcept
{
    return loc_;
}


token_error::token_error(location loc) noexcept
    : token(loc)
{
}

enum token::type token_error::type() const noexcept
{
    return type::ERROR;
}


token_identifier::token_identifier(location loc, std::string name)
    : token(loc)
    , name_(std::move(name))
{
}

const std::string& token_identifier::name() const
{
    return name_;
}

enum token::type token_identifier::type() const noexcept
{
    return type::IDENTIFIER;
}


token_operator::token_operator(location loc, enum kind k) noexcept
    : token(loc)
    , kind_(k)
{
}

const enum token_operator::kind& token_operator::kind() const
{
    return kind_;
}

enum token::type token_operator::type() const noexcept
{
    return type::OPERATOR;
}


token_end::token_end(location loc) noexcept
    : token(loc)
{
}

enum token::type token_end::type() const noexcept
{
    return type::END;
}


const token_operator* match_operator_kind(const token& tok, enum token_operator::kind kind)
{
    const auto op = dynamic_cast<const token_operator*>(&tok);
    if (op == nullptr)
        return nullptr;

    if (op->kind() != kind)
        return nullptr;

    return op;
}
