#include <relation.hpp>
#include <histhash.hpp>
#include <index.hpp>
#include <utility>
#include <fstream>
#include <iomanip>

#define RADIX (3)
#define RANGE (2 * 2 * 2)

RHJ::Relation::Relation()
:
tuples(nullptr),
size(0UL)
{
}

RHJ::Relation::Relation(Tuple * tuples, std::size_t size)
:
tuples(tuples),
size(size)
{
}

RHJ::Relation::Relation(Relation&& other) noexcept
:
tuples(std::move(other.tuples)),
size(std::move(other.size))
{
}

RHJ::Relation::~Relation()
{
    if (tuples)
        delete[] tuples;
}

RHJ::Relation& RHJ::Relation::operator=(Relation&& other) noexcept
{
    tuples = std::move(other.tuples);
    size   = std::move(other.size);

    return *this;
}

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Relation::Tuple& tuple)
{
    os
    << std::setw(10) << std::setfill(' ') << std::left << tuple.key
    << "|"
    << std::setw(10) << std::setfill(' ') << std::left << tuple.payload;

    return os;
}

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Relation& relation)
{
    os << "+----------+----------+" << std::endl;
    os << "|Key       |Value     |" << std::endl;
    os << "+----------+----------+" << std::endl;
    for (std::size_t row = 0UL; row < relation.size; row++)
        os << "|" << relation.tuples[row] << "|" << std::endl;

    os << "+----------+----------+" << std::endl;
    
    return os;
}

RHJ::List RHJ::Relation::RadixHashJoin(const RHJ::Relation& relR, const RHJ::Relation& relS) {

    PsumTable hashTableR(relR, RADIX);

    PsumTable hashTableS(relS, RADIX);

    List results;
    for (std::size_t hash = 0UL; hash < RANGE; hash++)
    {
        PsumTable::Bucket r(hashTableR[hash]);
        PsumTable::Bucket s(hashTableS[hash]);

        if (!r.size || !s.size)
            continue;

        if (r.size < s.size)
        {
            Index index(r); index.join(s, results);
        }
        else
        {
            Index index(s); index.join(r, results);
        }
    }

    return results;
}
