#ifndef SIMPLIFIER_HPP
#define SIMPLIFIER_HPP

#include <memory>

#include "expression.hpp"

std::unique_ptr<expression> simplify(const expression& expr);

#endif
