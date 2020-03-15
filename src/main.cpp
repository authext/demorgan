#include <fstream>
#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "simplifier.hpp"

int main(int argc, char** argv)
{
    std::ifstream in(argv[1]);

    lexer lex(in);
    parser par(lex);

    for (;;)
    {
        const auto expr = par.parse_expression();
        if (expr == nullptr)
            return 0;

        std::cout << "Loaded expression:" << std::endl;
        std::cout << fmt::format("{0:d}\n{0}", *expr) << std::endl;

        std::cout << std::endl;

        auto negated = make_unary(expression_unary::kind::NOT, simplify(*expr));
        auto final = make_unary(expression_unary::kind::NOT, simplify(*negated));

        std::cout << "Demorganized expression:" << std::endl;
        std::cout << fmt::format("{0:d}\n{0}", *final) << std::endl;
    }
}
