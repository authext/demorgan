#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>

#include <iosfwd>
#include <string>
#include <variant>

#include <fmt/format.h>

#include "position.hpp"

class token
{
public:
    enum class type
    {
        ERROR,
        IDENTIFIER,
        OPERATOR,
        END,
    };

    virtual ~token() = default;

    [[nodiscard]] const location& loc() const noexcept;
    [[nodiscard]] virtual type type() const noexcept = 0;

protected:
    location loc_;

    explicit token(location loc) noexcept;
};

class token_error final : public token
{
public:
    explicit token_error(location loc) noexcept;
    ~token_error() override = default;

    [[nodiscard]] enum type type() const noexcept override;
};

class token_identifier final : public token
{
public:
    token_identifier(location loc, std::string name);
    ~token_identifier() override = default;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] enum type type() const noexcept override;

private:
    std::string name_;
};

class token_operator final : public token
{
public:
    enum class kind
    {
        AMPERAMPER,
        PIPEPIPE,
        EXCLAM,
        LPAREN,
        RPAREN,
    };

    token_operator(location loc, kind k) noexcept;
    ~token_operator() override = default;

    [[nodiscard]] const kind& kind() const;

    [[nodiscard]] enum type type() const noexcept override;

private:
    enum kind kind_;
};

class token_end final : public token
{
public:
    explicit token_end(location loc) noexcept;
    ~token_end() override = default;

    [[nodiscard]] enum type type() const noexcept override;
};


const token_operator* match_operator_kind(const token& tok, enum token_operator::kind kind);


namespace fmt
{
template<>
struct formatter<token_error>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const token_error& err, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}: ERROR", err.loc());
    }
};

template<>
struct formatter<token_identifier>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const token_identifier& ident, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}: IDENT {}", ident.loc(), ident.name());
    }
};

template<>
struct formatter<token_operator>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const token_operator& op, FormatContext& ctx)
    {
        const auto op_str = ([&op]() {
            switch (op.kind())
            {
            case token_operator::kind::EXCLAM:
                return "!";

            case token_operator::kind::AMPERAMPER:
                return "&&";

            case token_operator::kind::PIPEPIPE:
                return "||";

            case token_operator::kind::LPAREN:
                return "(";

            case token_operator::kind::RPAREN:
                return ")";
            }
        })();

        return format_to(ctx.out(), "{}: OP {}", op.loc(), op_str);
    }
};

template<>
struct formatter<token_end>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const token_end& end, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}: END", end.loc());
    }
};

template<>
struct formatter<token>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const token& tok, FormatContext& ctx)
    {
        switch (tok.type())
        {
        case token::type::OPERATOR:
        {
            const auto tok_op = dynamic_cast<const token_operator&>(tok);
            return format_to(ctx.out(), "{}", tok_op);
        }

        case token::type::IDENTIFIER:
        {
            const auto tok_ident = dynamic_cast<const token_identifier&>(tok);
            return format_to(ctx.out(), "{}", tok_ident);
        }

        case token::type::END:
        {
            const auto tok_end = dynamic_cast<const token_end&>(tok);
            return format_to(ctx.out(), "{}", tok_end);
        }

        case token::type::ERROR:
        {
            const auto tok_err = dynamic_cast<const token_error&>(tok);
            return format_to(ctx.out(), "{}", tok_err);
        }
        }
    }
};
} // namespace fmt

#endif
