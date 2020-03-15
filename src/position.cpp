#include "position.hpp"

#include <ostream>

#include <fmt/format.h>

position::position(std::uint32_t line, std::uint32_t column) noexcept
    : line_(line)
    , column_(column)
{
}

const std::uint32_t& position::line() const noexcept
{
    return line_;
}

const std::uint32_t& position::column() const noexcept
{
    return column_;
}


location::location(position start, position end) noexcept
    : start_(start)
    , end_(end)
{
}

const position& location::start() const noexcept
{
    return start_;
}

const position& location::end() const noexcept
{
    return end_;
}
