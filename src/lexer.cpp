#include "lexer.hpp"

#include <climits>

#include <array>
#include <iostream>
#include <limits>
#include <string_view>

namespace
{
constexpr std::size_t cpow(std::size_t base, std::size_t exp)
{
    if (exp == 1)
        return base;
    else
        return base * cpow(base, exp - 1);
}

constexpr auto LEN = cpow(2, CHAR_BIT);
using table = std::array<std::array<lexer::state, LEN>, LEN>;

constexpr auto END = static_cast<unsigned char>(EOF);

constexpr std::string_view ALPHANUM_START = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
constexpr std::string_view ALPHANUM_REST
    = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";
constexpr std::string_view WHITESPACE = " \t\v\r\n";

constexpr void init_alphanum(table& tbl)
{
    for (const auto& s : ALPHANUM_START)
        tbl['\0'][s] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};

    for (const auto& s : ALPHANUM_START)
        for (const auto& r : ALPHANUM_REST)
            tbl[s][r] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};
    for (const auto& r1 : ALPHANUM_REST)
        for (const auto& r2 : ALPHANUM_REST)
            tbl[r1][r2] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};
    for (const auto& r : ALPHANUM_REST)
    {
        for (const auto& w : WHITESPACE)
            tbl[r][w] = {lexer::table_state::ACCEPT, lexer::accept_state::IDENTIFIER};

        tbl[r]['&'] = {lexer::table_state::ACCEPT, lexer::accept_state::IDENTIFIER};
        tbl[r]['|'] = {lexer::table_state::ACCEPT, lexer::accept_state::IDENTIFIER};
        tbl[r]['('] = {lexer::table_state::ACCEPT, lexer::accept_state::IDENTIFIER};
        tbl[r][')'] = {lexer::table_state::ACCEPT, lexer::accept_state::IDENTIFIER};
        tbl[r]['!'] = {lexer::table_state::ACCEPT, lexer::accept_state::IDENTIFIER};

        tbl[r][END] = {lexer::table_state::ACCEPT, lexer::accept_state::IDENTIFIER};
    }
}

constexpr void init_lparen(table& tbl)
{
    tbl['\0']['('] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};

    for (const auto& r : ALPHANUM_START)
        tbl['('][r] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    for (const auto& w : WHITESPACE)
        tbl['('][w] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['(']['&'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['(']['|'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['(']['('] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['('][')'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['(']['!'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['('][END] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
}

constexpr void init_rparen(table& tbl)
{
    tbl['\0'][')'] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};

    for (const auto& r : ALPHANUM_START)
        tbl[')'][r] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    for (const auto& w : WHITESPACE)
        tbl[')'][w] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl[')']['&'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl[')']['|'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl[')']['('] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl[')'][')'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl[')']['!'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl[')'][END] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
}

constexpr void init_amper(table& tbl)
{
    tbl['\0']['&'] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};

    for (const auto& r : ALPHANUM_START)
        tbl['&'][r] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    for (const auto& w : WHITESPACE)
        tbl['&'][w] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['&']['&'] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};
    tbl['&']['|'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['&']['('] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['&'][')'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['&']['!'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['&'][END] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
}

constexpr void init_bar(table& tbl)
{
    tbl['\0']['|'] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};

    for (const auto& r : ALPHANUM_START)
        tbl['|'][r] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    for (const auto& w : WHITESPACE)
        tbl['|'][w] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['|']['&'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['|']['|'] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};
    tbl['|']['('] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['|'][')'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['|']['!'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['|'][END] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
}

constexpr void init_exclam(table& tbl)
{
    tbl['\0']['!'] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};

    for (const auto& r : ALPHANUM_START)
        tbl['!'][r] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    for (const auto& w : WHITESPACE)
        tbl['!'][w] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['!']['&'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['!']['|'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['!']['('] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['!'][')'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
    tbl['!']['!'] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};

    tbl['!'][END] = {lexer::table_state::ACCEPT, lexer::accept_state::OPERATOR};
}

constexpr void init_whitespace(table& tbl)
{
    for (const auto& w : WHITESPACE)
        tbl['\0'][w] = {lexer::table_state::CONTINUE, lexer::accept_state::NONE};
    for (const auto& w : WHITESPACE)
    {
        for (const auto& r : ALPHANUM_START)
            tbl[w][r] = {lexer::table_state::ACCEPT, lexer::accept_state::WHITESPACE};

        tbl[w]['&'] = {lexer::table_state::ACCEPT, lexer::accept_state::WHITESPACE};
        tbl[w]['|'] = {lexer::table_state::ACCEPT, lexer::accept_state::WHITESPACE};
        tbl[w]['('] = {lexer::table_state::ACCEPT, lexer::accept_state::WHITESPACE};
        tbl[w][')'] = {lexer::table_state::ACCEPT, lexer::accept_state::WHITESPACE};
        tbl[w]['!'] = {lexer::table_state::ACCEPT, lexer::accept_state::WHITESPACE};

        tbl[w][END] = {lexer::table_state::ACCEPT, lexer::accept_state::WHITESPACE};
    }
}

constexpr table create_table()
{
    table tbl = {};
    for (auto& row : tbl)
        for (auto& col : row)
            col = {lexer::table_state::REJECT, lexer::accept_state::NONE};

    init_alphanum(tbl);
    init_lparen(tbl);
    init_rparen(tbl);
    init_amper(tbl);
    init_bar(tbl);
    init_exclam(tbl);
    init_whitespace(tbl);

    return tbl;
}

constexpr table tbl = create_table();

position step(const position& pos, char ch)
{
    if (ch == '\n')
    {
        return position(pos.line() + 1, 1);
    }
    else
    {
        return position(pos.line(), pos.column() + 1);
    }
}
} // namespace

lexer::lexer(std::istream& stream)
    : stream_(stream)
    , current_pos(1, 1)
    , prev_char(read())
{
}

std::unique_ptr<token> lexer::next_token()
{
    position start_pos = current_pos;

    std::string text;
    text += prev_char;

    for (;;)
    {
        char ch = prev_char;
        if (ch == static_cast<char>(EOF))
        {
            return std::make_unique<token_end>(location(current_pos, current_pos));
        }

        const auto& cur_row = tbl[ch];

        ch = read();

        const auto cur_col = cur_row[ch];

        switch (cur_col.tbl)
        {
        case table_state::REJECT:
            std::cerr << fmt::format(
                "{}: Error: Character `{0}' (0x{0:x} cannot follow `{1}' (0x{1:x}\n",
                current_pos,
                ch,
                text);
            return std::make_unique<token_error>(location(current_pos, current_pos));

        case table_state::ACCEPT:
        {
            const auto prev_pos = current_pos;
            current_pos = step(current_pos, ch);
            prev_char = ch;

            switch (cur_col.acc)
            {
            case accept_state::WHITESPACE:
                return next_token();

            case accept_state::OPERATOR:
            {
                const auto loc = location(start_pos, prev_pos);

                if (text == "!")
                {
                    return std::make_unique<token_operator>(loc, token_operator::kind::EXCLAM);
                }
                else if (text == "(")
                {
                    return std::make_unique<token_operator>(loc, token_operator::kind::LPAREN);
                }
                else if (text == ")")
                {
                    return std::make_unique<token_operator>(loc, token_operator::kind::RPAREN);
                }
                else if (text == "&&")
                {
                    return std::make_unique<token_operator>(loc, token_operator::kind::AMPERAMPER);
                }
                else if (text == "||")
                {
                    return std::make_unique<token_operator>(loc, token_operator::kind::PIPEPIPE);
                }
                else
                {
                    std::cerr << fmt::format("Invalid operator `{}'\n", text);
                    return std::make_unique<token_error>(loc);
                }
            }

            case accept_state::IDENTIFIER:
                return std::make_unique<token_identifier>(location(start_pos, prev_pos), text);

            case accept_state::NONE:
                assert(!"Impossible accept state");
            }
        }

        case table_state::CONTINUE:
            prev_char = ch;
            text += ch;
            break;
        }
    }
}

char lexer::read()
{
    char ch;
    if (!stream_.read(&ch, 1))
        return EOF;
    return ch;
}
