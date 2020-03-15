#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <cassert>

#include <memory>
#include <string>
#include <variant>
#include <map>

#include <fmt/format.h>

class expression
{
public:
    enum class type
    {
        BINARY,
        UNARY,
        IDENTIFIER,
    };

    expression(expression&&) = default;
    expression(const expression&) = default;
    virtual ~expression() = default;

    [[nodiscard]] virtual std::unique_ptr<expression> clone() const = 0;
    [[nodiscard]] virtual enum type type() const noexcept = 0;

protected:
    explicit expression() noexcept;
};

class expression_binary final : public expression
{
public:
    enum class kind
    {
        AND,
        OR,
    };

    expression_binary(kind op, std::unique_ptr<expression> left, std::unique_ptr<expression> right);
    expression_binary(const expression_binary& src);
    expression_binary(expression_binary&&) = default;
    ~expression_binary() override = default;

    [[nodiscard]] const kind& op() const noexcept;
    [[nodiscard]] const expression& left() const noexcept;
    [[nodiscard]] const expression& right() const noexcept;

    [[nodiscard]] std::unique_ptr<expression> clone() const override;
    [[nodiscard]] enum type type() const noexcept override;

private:
    kind op_;
    std::unique_ptr<expression> left_;
    std::unique_ptr<expression> right_;
};

class expression_unary final : public expression
{
public:
    enum class kind
    {
        NOT,
    };

    expression_unary(kind op, std::unique_ptr<expression> inner);
    expression_unary(const expression_unary& src);
    expression_unary(expression_unary&&) = default;
    ~expression_unary() override = default;

    [[nodiscard]] const kind& op() const noexcept;
    [[nodiscard]] const expression& inner() const noexcept;

    [[nodiscard]] std::unique_ptr<expression> clone() const override;
    [[nodiscard]] enum type type() const noexcept override;

private:
    kind op_;
    std::unique_ptr<const expression> inner_;
};

class expression_identifier final : public expression
{
public:
    explicit expression_identifier(std::string name);
    expression_identifier(const expression_identifier& src);
    expression_identifier(expression_identifier&&) = default;
    ~expression_identifier() override = default;

    [[nodiscard]] const std::string& name() const noexcept;

    [[nodiscard]] std::unique_ptr<expression> clone() const override;
    [[nodiscard]] enum type type() const noexcept override;

private:
    std::string name_;
};


bool operator==(const expression& l, const expression& r);
bool operator!=(const expression& l, const expression& r);
bool operator==(const expression_binary& l, const expression_binary& r);
bool operator!=(const expression_binary& l, const expression_binary& r);
bool operator==(const expression_unary& l, const expression_unary& r);
bool operator!=(const expression_unary& l, const expression_unary& r);
bool operator==(const expression_identifier& l, const expression_identifier& r);
bool operator!=(const expression_identifier& l, const expression_identifier& r);


std::unique_ptr<expression_binary> make_binary(
    expression_binary::kind kind,
    std::unique_ptr<expression> left,
    std::unique_ptr<expression> right);
std::unique_ptr<expression_unary>
make_unary(expression_unary::kind kind, std::unique_ptr<expression> inner);
std::unique_ptr<expression_identifier> make_identifier(std::string name);


namespace fmt
{
constexpr inline auto parse_fmt(format_parse_context& ctx, bool& debug, long& offset)
{
    offset = 0;
    debug = false;

    auto it = ctx.begin();
    auto end = ctx.end();

    if (it != end && *it == 'd')
    {
        debug = true;
        it++;
    }

    while (it != end && (std::isdigit(*it)))
        offset = 10 * offset + (*it++ - '0');

    // Check if reached the end of the range:
    if (it != end && *it != '}')
        throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
}

inline std::string subformat_debug(long offset, const expression& expr)
{
    const auto subf = fmt::format("{{:d{0}}}", offset);
    return fmt::format(subf, expr);
}

inline std::string subformat(enum expression::type parent_type, const expression& expr)
{
    const auto subf = ([&]() {
        switch (parent_type)
        {
        case expression::type::IDENTIFIER:
            assert(!"Identifier doesn't have subexpressions");
            return std::string();

        case expression::type::UNARY:
        case expression::type::BINARY:
            switch (expr.type())
            {
            case expression::type::IDENTIFIER:
            case expression::type::UNARY:
                return fmt::format("{{}}");

            case expression::type::BINARY:
                return fmt::format("({{}})");
            }
        }
    })();

    return fmt::format(subf, expr);
}


template<>
struct formatter<expression_binary>
{
    long offset = 0;
    bool debug = false;

    constexpr auto parse(format_parse_context& ctx)
    {
        return parse_fmt(ctx, debug, offset);
    }

    template<typename FormatContext>
    auto format(const expression_binary& expr_bin, FormatContext& ctx)
    {
        if (debug)
        {
            const auto op_str = ([&expr_bin]() {
                switch (expr_bin.op())
                {
                case expression_binary::kind::AND:
                    return "AND";

                case expression_binary::kind::OR:
                    return "OR";
                }
            })();

            std::string offset_str;
            offset_str.append(offset, ' ');

            const auto left = subformat_debug(offset + 2, expr_bin.left());
            const auto right = subformat_debug(offset + 2, expr_bin.right());
            return format_to(ctx.out(), "{0}({1}\n{2}\n{3})", offset_str, op_str, left, right);
        }
        else
        {
            const auto op_str = ([&expr_bin]() {
                switch (expr_bin.op())
                {
                case expression_binary::kind::AND:
                    return "&&";

                case expression_binary::kind::OR:
                    return "||";
                }
            })();

            const auto left = subformat(expression::type::BINARY, expr_bin.left());
            const auto right = subformat(expression::type::BINARY, expr_bin.right());
            return format_to(ctx.out(),"{0} {1} {2}", left, op_str, right);
        }
    }
};

template<>
struct formatter<expression_unary>
{
    long offset = 0;
    bool debug = false;

    constexpr auto parse(format_parse_context& ctx)
    {
        return parse_fmt(ctx, debug, offset);
    }

    template<typename FormatContext>
    auto format(const expression_unary& expr_un, FormatContext& ctx)
    {
        if (debug)
        {
            const auto op_str = ([&expr_un]() {
                switch (expr_un.op())
                {
                case expression_unary::kind::NOT:
                    return "NOT";
                }
            })();

            std::string offset_str;
            offset_str.append(offset, ' ');

            const auto inner = subformat_debug(offset + 2, expr_un.inner());
            return format_to(ctx.out(), "{0}({1}\n{2})", offset_str, op_str, inner);
        }
        else
        {
            const auto op_str = ([&expr_un]() {
                switch (expr_un.op())
                {
                case expression_unary::kind::NOT:
                    return "!";
                }
            })();

            const auto inner = subformat(expression::type::UNARY, expr_un.inner());
            return format_to(
                ctx.out(),
                "{0}{1}",
                op_str,
                inner);
        }
    }
};

template<>
struct formatter<expression_identifier>
{
    long offset = 0;
    bool debug = false;

    constexpr auto parse(format_parse_context& ctx)
    {
        return parse_fmt(ctx, debug, offset);
    }

    template<typename FormatContext>
    auto format(const expression_identifier& expr_ident, FormatContext& ctx)
    {
        if (debug)
        {
            std::string offset_str;
            offset_str.append(offset, ' ');

            return format_to(ctx.out(), "{}(ID {})", offset_str, expr_ident.name());
        }
        else
        {
            return format_to(ctx.out(), "{}", expr_ident.name());
        }
    }
};

template<>
struct formatter<expression>
{
    std::string_view fmt_string = "";

    constexpr auto parse(format_parse_context& ctx)
    {
        size_t len = 0;

        auto it = ctx.begin();
        auto end = ctx.end();

        while (it != end && *it != '}')
        {
            len++;
            it++;
        }

        // Check if reached the end of the range:
        if (it != end && *it != '}')
            throw format_error("invalid format");

        fmt_string = {ctx.begin(), len};

        // Return an iterator past the end of the parsed range:
        return it;
    }

    template<typename FormatContext>
    auto format(const expression& expr, FormatContext& ctx)
    {
        const auto subformat = fmt::format("{{:{}}}", fmt_string);

        if (const auto* expr_bin = dynamic_cast<const expression_binary*>(&expr))
        {
            return format_to(ctx.out(), subformat, *expr_bin);
        }
        else if (const auto* expr_un = dynamic_cast<const expression_unary*>(&expr))
        {
            return format_to(ctx.out(), subformat, *expr_un);
        }
        else if (const auto* expr_ident = dynamic_cast<const expression_identifier*>(&expr))
        {
            return format_to(ctx.out(), subformat, *expr_ident);
        }
        else
        {
            assert(!"Cannot format an `expression'");
            return ctx.out();
        }
    }
};
} // namespace fmt

#endif
