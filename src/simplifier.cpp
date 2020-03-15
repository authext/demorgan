#include "simplifier.hpp"

std::unique_ptr<expression> simplify(const expression_unary& unary);
std::unique_ptr<expression> simplify(const expression_binary& binary);
std::unique_ptr<expression> simplify(const expression_identifier& ident);

/**
 * Simplifies the given expression as much as possible
 *
 * The function implements the following rewrite rules:
 * * NOT(NOT(<EXPR>)) -> <EXPR>
 * * NOT(<EXPR1> AND <EXPR2>) -> NOT(<EXPR1>) OR NOT(<EXPR2>)
 * * NOT(<EXPR1> OR <EXPR2>) -> NOT(<EXPR1>) AND NOT(<EXPR2>)
 * * <EXPR1> [AND/OR] <EXPR1> -> <EXPR1>
 *
 * All rewrite rules are performed with recursive descent, from
 * left to right, as much as possible.
 *
 * @param expr The expression to simplify
 * @return Owning reference to simplified expression
 */
std::unique_ptr<expression> simplify(const expression& expr)
{
    switch (expr.type())
    {
    case expression::type::IDENTIFIER:
    {
        const auto& ident = dynamic_cast<const expression_identifier&>(expr);
        return simplify(ident);
    }

    case expression::type::BINARY:
    {
        const auto& binary = dynamic_cast<const expression_binary&>(expr);
        return simplify(binary);
    }

    case expression::type::UNARY:
    {
        const auto& unary = dynamic_cast<const expression_unary&>(expr);
        return simplify(unary);
    }
    }
}

std::unique_ptr<expression> simplify(const expression_unary& unary)
{
    switch (unary.op())
    {
    case expression_unary::kind::NOT:
    {
        auto simp_inner = simplify(unary.inner());

        switch (simp_inner->type())
        {
        case expression::type::UNARY:
        {
            const auto& simp_unary = dynamic_cast<const expression_unary&>(*simp_inner);
            switch (simp_unary.op())
            {
            case expression_unary::kind::NOT:
                return simp_unary.inner().clone();
            }
        }

        case expression::type::BINARY:
        {
            const auto simp_binary = dynamic_cast<const expression_binary&>(*simp_inner);
            auto& left = simp_binary.left();
            auto& right = simp_binary.right();

            const auto neg_left = make_unary(expression_unary::kind::NOT, left.clone());
            const auto neg_right = make_unary(expression_unary::kind::NOT, right.clone());

            auto simp_left = simplify(*neg_left);
            auto simp_right = simplify(*neg_right);

            switch (simp_binary.op())
            {
            case expression_binary::kind::AND:
                return make_binary(
                    expression_binary::kind::OR, std::move(simp_left), std::move(simp_right));

            case expression_binary::kind::OR:
                return make_binary(
                    expression_binary::kind::AND, std::move(simp_left), std::move(simp_right));
            }
        }

        case expression::type::IDENTIFIER:
        {
            return make_unary(expression_unary::kind::NOT, std::move(simp_inner));
        }
        }
    }
    }
}

std::unique_ptr<expression> simplify(const expression_binary& binary)
{
    auto simp_left = simplify(binary.left());
    auto simp_right = simplify(binary.right());

    if (*simp_left == *simp_right)
    {
        return simp_left;
    }
    else
    {
        return binary.clone();
    }
}

std::unique_ptr<expression> simplify(const expression_identifier& ident)
{
    return ident.clone();
}
