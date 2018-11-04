#include <relation.hpp>
#include <histhash.hpp>
#include <index.hpp>
#include <cmath>
#include <fstream>          // std::ostream

#if defined(__VERBOSE__)
    #include <iomanip>          // std::setw, std::setfill, std::left
#endif

#if !defined(__CACHE_SIZE__)
    #define __CACHE_SIZE__ (32UL * 1024UL)
#endif

static inline void calc_range(std::size_t rSize, size_t sSize, std::size_t& range, radix_t& radix)
{
    rSize *= sizeof(RHJ::Relation::Tuple);
    sSize *= sizeof(RHJ::Relation::Tuple);

    std::size_t maxSize = (rSize > sSize ? rSize : sSize);

    range = static_cast<std::size_t>(std::ceil(static_cast<double>(maxSize) / static_cast<double>(__CACHE_SIZE__)));

    radix = static_cast<radix_t>(std::ceil(std::log2(range)));

    range = static_cast<std::size_t>(std::pow(2UL, radix));
}

#if defined(__VERBOSE__)
    std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Relation::Tuple& tuple)
    {
        os
        << std::setw(10) << std::setfill(' ') << std::left << tuple.key
        << "|"
        << std::setw(10) << std::setfill(' ') << std::left << tuple.payload;

        return os;
    }
#endif

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Relation& relation)
{
    #if defined(__VERBOSE__)
        os << "+----------+----------+" << std::endl;
        os << "|Key       |Value     |" << std::endl;
        os << "+----------+----------+" << std::endl;
        for (std::size_t row = 0UL; row < relation.size; row++)
            os << "|" << relation.tuples[row] << "|" << std::endl;

        os << "+----------+----------+" << std::endl;
        os << "\n[\"" << relation.name << "\"]: " << relation.size  << " entries...";
    #else
        os << relation.size << " entries...";
    #endif

    return os;
}

RHJ::List RHJ::Relation::RadixHashJoin(const RHJ::Relation& relR, const RHJ::Relation& relS) {

    radix_t radix; std::size_t range;

    calc_range(relR.size, relS.size, range, radix);

    PsumTable hashTableR(relR, radix, range);

    PsumTable hashTableS(relS, radix, range);

    List results(relR, relS);

    for (std::size_t hash = 0UL; hash < range; hash++)
    {
        PsumTable::Bucket r(hashTableR[hash]);
        PsumTable::Bucket s(hashTableS[hash]);

        if (!r.size || !s.size)
            continue;

        if (r.size < s.size)
        {
            Index index(r); index.join(s, results, hashTableR);
        }
        else
        {
            Index index(s); index.join(r, results, hashTableR);
        }
    }

    return results;
}
