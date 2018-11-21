
#include <query.hpp>
#include <cstdlib>      // std::strtol
#include <cstring>      // std::strtok
#include <fstream>      // std::ostream

// Operand Implementation
RHJ::Query::Predicate::Operand& RHJ::Query::Predicate::Operand::operator=(char * str)
{
    char * end;

    rel = std::strtoul(str, &end, 10);
    col = std::strtoul(&end[1UL], nullptr, 10);

    return *this;
}

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Query::Predicate::Operand& operand)
{
    return os << operand.rel << '.' << operand.col;
}

// Predicate Implementation
RHJ::Query::Predicate& RHJ::Query::Predicate::operator=(char * str)
{
    for (const char * current = str; *current; current++)
    {
        switch (*current)
        {
            case '=': type = filter_eq_t; break;
            case '<': type = filter_lt_t; break;
            case '>': type = filter_gt_t; break;
            case '.': type = join_t;      break;
        }
    }

    char * lstr = std::strtok(str,     "=<>");
    char * rstr = std::strtok(nullptr, "=<>");

    left = lstr;

    if (type != join_t)
    {
        right.constraint = std::strtol(rstr, nullptr, 10);
    }
    else
    {
        right.operand = rstr;
    }

    return *this;
}

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Query::Predicate& predicate)
{
    os << predicate.left;

    switch (predicate.type)
    {
        case Query::Predicate::join_t:      os << '=' << predicate.right.operand;    break;
        case Query::Predicate::filter_eq_t: os << '=' << predicate.right.constraint; break;
        case Query::Predicate::filter_lt_t: os << '<' << predicate.right.constraint; break;
        case Query::Predicate::filter_gt_t: os << '>' << predicate.right.constraint; break;
    }

    return os;
}

// Query Implementation
#define COUNT_INSTANCES(str, c, counter)                    \
do                                                          \
{                                                           \
    for (const char * current = str; *current; current++)   \
        if (*current == c)                                  \
            counter++;                                      \
                                                            \
} while (false)                                             \

#define TOKENIZE(str, c, tokens)                            \
do                                                          \
{                                                           \
    std::size_t i = 0UL;                                    \
                                                            \
    char * token = std::strtok(str, c);                     \
    do                                                      \
    {                                                       \
        tokens[i++] = token;                                \
    } while ((token = std::strtok(nullptr, c)));            \
                                                            \
} while (false)                                             \

#define PARSE(str, delimeter, targets, count)               \
do                                                          \
{                                                           \
    char ** tokens = new char*[count];                      \
                                                            \
    TOKENIZE(str, delimeter, tokens);                       \
                                                            \
    for (std::size_t i = 0UL; i < count; i++)               \
        targets[i] = tokens[i];                             \
                                                            \
    delete[] tokens;                                        \
                                                            \
} while (false)                                             \

RHJ::Query::Query(char * query)
:
relCount(1UL),
preCount(1UL),
cheCount(1UL),
relations(nullptr),
predicates(nullptr),
checksums(nullptr)
{
    char * qtokens[3UL]; TOKENIZE(query, "|", qtokens);

    COUNT_INSTANCES(qtokens[0UL], ' ', relCount);
    COUNT_INSTANCES(qtokens[1UL], '&', preCount);
    COUNT_INSTANCES(qtokens[2UL], ' ', cheCount);

    relations  = new std::size_t[relCount];
    predicates = new Predicate[preCount];
    checksums  = new Checksum[cheCount];

    char * end = qtokens[0UL];
    for (std::size_t i = 0UL; i < relCount; i++)
        relations[i] = std::strtol(end, &end, 10);

    PARSE(qtokens[1UL], "&", predicates, preCount);
    PARSE(qtokens[2UL], " ", checksums,  cheCount);
}

#define FEED(os, targets, count, delimeter)                 \
do                                                          \
{                                                           \
    for (std::size_t i = 0UL; i < count; i++)               \
    {                                                       \
        os << targets[i];                                   \
                                                            \
        if (i != count - 1UL)                               \
            os << delimeter;                                \
    }                                                       \
} while (false)                                             \

std::ostream& RHJ::operator<<(std::ostream& os, const Query& query)
{
    FEED(os, query.relations, query.relCount, ' ');

    os << '|';

    FEED(os, query.predicates, query.preCount, '&');

    os << '|';

    FEED(os, query.checksums, query.cheCount, ' ');

    return os;
}
