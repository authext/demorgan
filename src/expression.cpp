#include "expression.hpp"

expression::expression() noexcept
{
}


expression_binary::expression_binary(
    kind op,
    std::unique_ptr<expression> left,
    std::unique_ptr<expression> right)
    : expression()
    , op_(op)
    , left_(std::move(left))
    , right_(std::move(right))
{
}

expression_binary::expression_binary(const expression_binary& src)
    : expression()
    , op_(src.op_)
    , left_(src.left_->clone())
    , right_(src.right_->clone())
{
}

const expression_binary::kind& expression_binary::op() const noexcept
{
    return op_;
}

const expression& expression_binary::left() const noexcept
{
    return *left_;
}

const expression& expression_binary::right() const noexcept
{
    return *right_;
}

std::unique_ptr<expression> expression_binary::clone() const
{
    return std::make_unique<expression_binary>(op_, left_->clone(), right_->clone());
}

enum expression::type expression_binary::type() const noexcept
{
    return type::BINARY;
}


expression_unary::expression_unary(kind op, std::unique_ptr<expression> inner)
    : expression()
    , op_(op)
    , inner_(std::move(inner))
{
}

expression_unary::expression_unary(const expression_unary& src)
    : expression()
    , op_(src.op_)
    , inner_(src.inner_->clone())
{
}

const expression_unary::kind& expression_unary::op() const noexcept
{
    return op_;
}

const expression& expression_unary::inner() const noexcept
{
    return *inner_;
}

std::unique_ptr<expression> expression_unary::clone() const
{
    return std::make_unique<expression_unary>(op_, inner_->clone());
}

enum expression::type expression_unary::type() const noexcept
{
    return type::UNARY;
}


expression_identifier::expression_identifier(std::string name)
    : expression()
    , name_(std::move(name))
{
}

expression_identifier::expression_identifier(const expression_identifier& src)
    : expression()
    , name_(src.name_)
{
}

const std::string& expression_identifier::name() const noexcept
{
    return name_;
}

std::unique_ptr<expression> expression_identifier::clone() const
{
    return std::make_unique<expression_identifier>(name_);
}

enum expression::type expression_identifier::type() const noexcept
{
    return type::IDENTIFIER;
}


bool operator==(const expression& l, const expression& r)
{
    if (l.type() != r.type())
        return false;

    switch (l.type())
    {
    case expression::type::BINARY:
    {
        const auto& l_binary = dynamic_cast<const expression_binary&>(l);
        const auto& r_binary = dynamic_cast<const expression_binary&>(r);
        return l_binary == r_binary;
    }

    case expression::type::UNARY:
    {
        const auto& l_unary = dynamic_cast<const expression_unary&>(l);
        const auto& r_unary = dynamic_cast<const expression_binary&>(r);
        return l_unary == r_unary;
    }

    case expression::type::IDENTIFIER:
    {
        const auto& l_ident = dynamic_cast<const expression_identifier&>(l);
        const auto& r_ident = dynamic_cast<const expression_identifier&>(r);
        return l_ident == r_ident;
    }
    }
}

bool operator!=(const expression& l, const expression& r)
{
    return !(l == r);
}

bool operator==(const expression_binary& l, const expression_binary& r)
{
    return l.op() == r.op() && l.left() == r.left() && l.right() == r.right();
}

bool operator!=(const expression_binary& l, const expression_binary& r)
{
    return !(l == r);
}

bool operator==(const expression_unary& l, const expression_unary& r)
{
    return l.op() == r.op() && l.inner() == r.inner();
}

bool operator!=(const expression_unary& l, const expression_unary& r)
{
    return !(l == r);
}

bool operator==(const expression_identifier& l, const expression_identifier& r)
{
    return l.name() == r.name();
}

bool operator!=(const expression_identifier& l, const expression_identifier& r)
{
    return !(l == r);
}


std::unique_ptr<expression_binary> make_binary(
    expression_binary::kind kind,
    std::unique_ptr<expression> left,
    std::unique_ptr<expression> right)
{
    return std::make_unique<expression_binary>(kind, std::move(left), std::move(right));
}

std::unique_ptr<expression_unary>
make_unary(expression_unary::kind kind, std::unique_ptr<expression> inner)
{
    return std::make_unique<expression_unary>(kind, std::move(inner));
}

std::unique_ptr<expression_identifier> make_identifier(std::string name)
{
    return std::make_unique<expression_identifier>(std::move(name));
}
