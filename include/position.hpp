#ifndef POSITION_HPP
#define POSITION_HPP

#include <cstdint>

#include <iosfwd>

#include <fmt/format.h>

class position final
{
public:
    position(std::uint32_t line, std::uint32_t column) noexcept;
    position(const position&) noexcept = default;
    position(position&&) noexcept = default;
    ~position() = default;

    position& operator=(const position&) noexcept = default;
    position& operator=(position&&) noexcept = default;

    [[nodiscard]] const std::uint32_t& line() const noexcept;
    [[nodiscard]] const std::uint32_t& column() const noexcept;

private:
    std::uint32_t line_;
    std::uint32_t column_;
};

class location final
{
public:
    location(position start, position end) noexcept;
    location(const location&) noexcept = default;
    location(location&&) noexcept = default;

    location& operator=(const location&) noexcept = default;
    location& operator=(location&&) noexcept = default;

    [[nodiscard]] const position& start() const noexcept;
    [[nodiscard]] const position& end() const noexcept;

private:
    position start_;
    position end_;
};

namespace fmt
{
template<>
struct formatter<position>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const position& pos, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}:{}", pos.line(), pos.column());
    }
};

template<>
struct formatter<location>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const location& loc, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}-{}", loc.start(), loc.end());
    }
};
} // namespace fmt

#endif
