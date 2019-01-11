
#include <statistics.hpp>
#include <query.hpp>
#include <meta.hpp>

#include <cmath>
#include <cassert>

#include <fstream>
#include <utility>
#include <unordered_map>
#include <vector>

std::ostream& RHJ::operator<<(std::ostream& os, const Statistics& s)
{
    return os
    << "{ l: " << s.l
    << ", u: " << s.u
    << ", f: " << s.f
    << ", d: " << s.d
    << " }";
}

#define MIN(a, b) (a < b ? a : b)

#define MAX(a, b) (a > b ? a : b)

#define UPDATE_OTHERS(METHOD, statistics, rel, col, ...)                            \
do                                                                                  \
{                                                                                   \
    for (std::size_t ocol = 0UL; ocol < statistics[rel].size(); ocol++)             \
        if (i != col)                                                               \
            METHOD(statistics, rel, ocol, __VA_ARGS__);                             \
} while (false);                                                                    \

#define FORMULA(a, b, c, d, e) (a * (1.0f - std::pow(1.0f - (b / c), d / e)))

#define FILTER_EQUALS_SELF(statistics, rel, col, k)                                 \
do                                                                                  \
{                                                                                   \
    Statistics& A = statistics[rel][col];                                           \
                                                                                    \
    A.l = k;                                                                        \
    A.u = k;                                                                        \
                                                                                    \
    std::size_t i;                                                                  \
    for (i = 0UL; i < meta[rel].columnSize; i++)                                    \
        if (meta[rel].columns[col][i] == k)                                         \
            break;                                                                  \
                                                                                    \
    if (i >= meta[rel].columnSize)                                                  \
    {                                                                               \
        A.d = A.f = 0.0f;                                                           \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        A.f /= A.d;                                                                 \
        A.d = 1.0f;                                                                 \
    }                                                                               \
} while (false)                                                                     \

#define FILTER_EQUALS_OTHERS(statistics, rel, col, A, A_old)                        \
do                                                                                  \
{                                                                                   \
    Statistics& C = statistics[rel][col];                                           \
                                                                                    \
    C.d = FORMULA(C.d, A.f, A_old.f, C.f, C.d);                                     \
    C.f = A.f;                                                                      \
} while (false)                                                                     \

#define FILTER_EQUALS_ALL(statistics, rel, col, k, old_self)                        \
do                                                                                  \
{                                                                                   \
    FILTER_EQUALS_SELF(statistics, rel, col, k);                                    \
                                                                                    \
    UPDATE_OTHERS                                                                   \
    (                                                                               \
        FILTER_EQUALS_OTHERS,                                                       \
        statistics, rel, col,                                                       \
        statistics[lrel][lcol], old_self                                            \
    );                                                                              \
} while (false)                                                                     \

#define FILTER_RANGE_SELF(A, k1, k2)                                                \
do                                                                                  \
{                                                                                   \
    A.l = k1;                                                                       \
    A.u = k2;                                                                       \
    A.d = ((k2 - k1) / (A.u - A.l)) * A.d;                                          \
    A.f = ((k2 - k1) / (A.u - A.l)) * A.f;                                          \
} while (false)                                                                     \

#define FILTER_RANGE_OTHERS(statistics, rel, col, A, A_old)                         \
FILTER_EQUALS_OTHERS(statistics, rel, col, A, A_old)                                \

#define FILTER_RANGE_ALL(statistics, rel, col, k1, k2, old_self)                    \
do                                                                                  \
{                                                                                   \
    FILTER_RANGE_SELF(statistics[rel][col], k1, k2);                                \
                                                                                    \
    UPDATE_OTHERS                                                                   \
    (                                                                               \
        FILTER_RANGE_OTHERS,                                                        \
        statistics, rel, col,                                                       \
        statistics[lrel][lcol], old_self                                            \
    );                                                                              \
                                                                                    \
} while (false)                                                                     \

#define FILTER_LESS_ALL(statistics, rel, col, k, old_self)                          \
FILTER_RANGE_ALL(statistics, rel, col, statistics[rel][col].l, k, old_self)         \

#define FILTER_GREATER_ALL(statistics, rel, col, k, old_self)                       \
FILTER_RANGE_ALL(statistics, rel, col, k, statistics[rel][col].u, old_self)         \

#define JOIN_SAME_RLTN_SELF(A, B)                                                   \
do                                                                                  \
{                                                                                   \
    float A_old_f = A.f;                                                            \
                                                                                    \
    A.l = B.l = MAX(A.l, B.l);                                                      \
    A.u = B.u = MIN(A.u, B.u);                                                      \
    A.f = B.f = A.f / (A.u - A.l + 1.0f);                                           \
    A.d = B.d = FORMULA(A.d, A.f, A_old_f, A_old_f, A.d);                           \
} while (false)                                                                     \

#define JOIN_SAME_RLTN_OTHERS(statistics, rel, lcol, rcol, A, A_old)                \
do                                                                                  \
{                                                                                   \
    for (std::size_t col = 0UL; col < statistics[rel].size(); col++)                \
        if (col != lcol && col != rcol)                                             \
            FILTER_EQUALS_OTHERS(statistics, rel, col, A, A_old);                   \
} while (false)                                                                     \

#define JOIN_SAME_RLTN_ALL(statistics, rel, lcol, rcol, old_self)                   \
do                                                                                  \
{                                                                                   \
    JOIN_SAME_RLTN_SELF(statistics[rel][lcol], statistics[rel][rcol]);              \
                                                                                    \
    JOIN_SAME_RLTN_OTHERS                                                           \
    (                                                                               \
        statistics, rel, lcol, rcol,                                                \
        statistics[rel][lcol], old_self                                             \
    );                                                                              \
} while (false)                                                                     \

#define JOIN_DIFF_RLTN_SELF(A, B)                                                   \
do                                                                                  \
{                                                                                   \
    float n = A.u - A.l + 1.0f;                                                     \
                                                                                    \
    A.f = B.f = (A.f * B.f) / n;                                                    \
    A.d = B.d = (A.d * B.d) / n;                                                    \
} while (false)                                                                     \

#define JOIN_DIFF_RLTN_OTHERS(statistics, rel, col, A, A_old)                       \
do                                                                                  \
{                                                                                   \
    Statistics& C = statistics[rel][col];                                           \
                                                                                    \
    C.f = A.f;                                                                      \
    C.d = FORMULA(C.d, A.d, A_old.d, C.f, C.d);                                     \
} while (false)                                                                     \

#define JOIN_DIFF_RLTN_ALL(statistics, lrel, lcol, lold, rrel, rcol, rold)          \
do                                                                                  \
{                                                                                   \
    JOIN_DIFF_RLTN_SELF(statistics[lrel][lcol], statistics[rrel][rcol]);            \
                                                                                    \
    UPDATE_OTHERS                                                                   \
    (                                                                               \
        JOIN_DIFF_RLTN_OTHERS,                                                      \
        statistics, lrel, lcol,                                                     \
        statistics[lrel][lcol], lold                                                \
    );                                                                              \
                                                                                    \
    UPDATE_OTHERS                                                                   \
    (                                                                               \
        JOIN_DIFF_RLTN_OTHERS,                                                      \
        statistics, rrel, rcol,                                                     \
        statistics[rrel][rcol], rold                                                \
    );                                                                              \
} while (false)                                                                     \

#define AUTOCORRELATION_SELF(A)                                                     \
do                                                                                  \
{                                                                                   \
    A.f = A.f * A.f / (A.u - A.l + 1.0f);                                           \
} while (false)                                                                     \

#define AUTOCORRELATION_OTHERS(statistics, rel, col, A)                             \
do                                                                                  \
{                                                                                   \
    Statistics& C = statistics[rel][col];                                           \
                                                                                    \
    C.f = A.f;                                                                      \
} while (false)                                                                     \

#define AUTOCORRELATION_ALL(statistics, rel, col)                                   \
do                                                                                  \
{                                                                                   \
    AUTOCORRELATION_SELF(statistics[rel][col]);                                     \
                                                                                    \
    UPDATE_OTHERS                                                                   \
    (                                                                               \
        AUTOCORRELATION_OTHERS,                                                     \
        statistics, rel, col,                                                       \
        statistics[rel][col]                                                        \
    );                                                                              \
} while (false)                                                                     \

#define MAX_BOOLS (10000000L)

#define calculate(statistics, rel, col)                                             \
do                                                                                  \
{                                                                                   \
    tuple_payload_t l = TUPLE_PAYLOAD_MAX;                                          \
    tuple_payload_t u = TUPLE_PAYLOAD_MIN;                                          \
                                                                                    \
    tuple_key_t f = meta[rel].rowSize, d = 0;                                       \
                                                                                    \
    for(tuple_key_t i = 0; i < f; i++) {                                            \
        tuple_payload_t item = meta[rel].columns[col][i];                           \
                                                                                    \
        if(item < l) l = item;                                                      \
                                                                                    \
        if(item > u) u = item;                                                      \
    }                                                                               \
                                                                                    \
    bool* bool_arr = new bool[ (u - l + 1) > MAX_BOOLS ? MAX_BOOLS : (u - l + 1) ]; \
                                                                                    \
    for(tuple_payload_t i = 0; i < (u - l + 1); i++)                                \
        bool_arr[i] = false;                                                        \
                                                                                    \
    for(tuple_key_t i = 0; i < f; i++) {                                            \
        tuple_payload_t item = meta[rel].columns[col][i];                           \
        if(!bool_arr[ ( item - l ) % MAX_BOOLS ])                                   \
        {                                                                           \
            bool_arr[ ( item - l ) % MAX_BOOLS ] = true;                            \
            d++;                                                                    \
        }                                                                           \
    }                                                                               \
                                                                                    \
    delete[] bool_arr;                                                              \
                                                                                    \
    statistics[rel][col] = Statistics(l, u, f, d);                                  \
} while (false)                                                                     \

RHJ::Statistics ** RHJ::statistics = nullptr;

void RHJ::Statistics::load()
{
    statistics = new Statistics*[Meta::total];

    for (std::size_t rel = 0UL; rel < Meta::total; rel++)
    {
        statistics[rel] = new Statistics[meta[rel].columnSize];
        
        for(std::size_t col = 0; col < meta[rel].columnSize; col++)
            calculate(statistics, rel, col);
    }
}

void RHJ::Statistics::dump()
{
    for (std::size_t i = 0UL; i < Meta::total; i++)
        delete[] statistics;

    delete[] statistics;
}

static struct
{
    std::unordered_map<std::size_t, std::vector<RHJ::Statistics>> statistics;

    const RHJ::Query * query;
} cache;

void RHJ::Statistics::preprocess(const Query& query)
{
    cache.statistics.clear();

    for (std::size_t i = 0UL; i < query.relCount; i++)
    {
        const std::size_t rel = query.relations[i];

        cache.statistics[rel] = std::vector<Statistics>(meta[rel].columnSize, Statistics());

        for (std::size_t col = 0UL; col < meta[rel].columnSize; col++)
            cache.statistics[rel][col] = statistics[rel][col];
    }

    for (std::size_t i = 0UL; i < query.preCount; i++)
    {
        const Query::Predicate& predicate = query.predicates[i];

        const tuple_key_t lrel = query.relations[predicate.left.rel],
                          lcol = query.relations[predicate.left.col];
        
        const Statistics lold(cache.statistics[lrel][lcol]);

        switch (predicate.type)
        {
            case Query::Predicate::Type::filter_eq_t:

                FILTER_EQUALS_ALL
                (
                    cache.statistics, lrel, lcol,
                    predicate.right.constraint, lold
                );

            break;

            case Query::Predicate::Type::filter_lt_t:

                FILTER_LESS_ALL
                (
                    cache.statistics, lrel, lcol,
                    predicate.right.constraint, lold
                );

            break;

            case Query::Predicate::Type::filter_gt_t:

                FILTER_GREATER_ALL
                (
                    cache.statistics, lrel, lcol,
                    predicate.right.constraint, lold
                );

            break;

            case Query::Predicate::Type::join_t:

                const tuple_key_t rrel = query.relations[predicate.right.operand.rel],
                                  rcol = query.relations[predicate.right.operand.col];

                assert(lrel == rrel && lcol != rcol);

                const Statistics rold(cache.statistics[rrel][rcol]);

                JOIN_SAME_RLTN_ALL(cache.statistics, lrel, lcol, rcol, lold);

            break;
        }
    }

    cache.query = &query;
}

float RHJ::Statistics::expected_cost
(
    const Query::Predicate * predicates, std::size_t preCount
)
{
    std::unordered_map<std::size_t, std::vector<RHJ::Statistics>> clone(cache.statistics);

    float cost = 0.0f;
    for (std::size_t i = 0UL; i < preCount; i++)
    {
        const Query::Predicate& predicate = predicates[i];

        assert(predicate.type == Query::Predicate::Type::join_t);

        const tuple_key_t lrel = cache.query->relations[predicate.left.rel],
                          lcol = cache.query->relations[predicate.left.col],
                          rrel = cache.query->relations[predicate.right.operand.rel],
                          rcol = cache.query->relations[predicate.right.operand.col];

        assert(lrel != rrel || lcol == rcol);

        const Statistics lold(clone[lrel][lcol]), rold(clone[rrel][rcol]);

        if (lrel != rrel)
            JOIN_DIFF_RLTN_ALL(clone, lrel, lcol, lold, rrel, rcol, rold);
        else
            AUTOCORRELATION_ALL(clone, lrel, lcol);

        cost += clone[rrel][rcol].f;
    }

    return cost;
}
