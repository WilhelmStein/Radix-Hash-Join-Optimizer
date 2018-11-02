#include <relation.hpp>
#include <histhash.hpp>
#include <index.hpp>
#include <fstream>          // std::ostream
#include <iomanip>          // std::setw, std::setfill, std::left
#include <cmath>

#define CACHE_SIZE 32
// #define RANGE(R_SIZE, S_SIZE, RADIX)                                                                    \
// (do{                                                                                                    \
//     size_t maxRelSize = ( ( (R_SIZE) > (S_SIZE) ) ? (R_SIZE) : (S_SIZE) );                              \
//     size_t range = ( maxRelsize / ( ( (maxRelsize) > (CACHE_SIZE) ) ? (CACHE_SIZE) : (maxRelSize) ) );  \
//     RADIX = ( (radix_t) log2(range) );                                                                  \
//     range;                                                                                              \                                \
// }while(false);)

static inline size_t calc_range(size_t rSize, size_t sSize, radix_t& radix)
{
    size_t maxRelSize = ( ( (rSize) > (sSize) ) ? (rSize) : (sSize) );                              
    size_t range = ( maxRelSize / ( ( (maxRelSize) > (CACHE_SIZE) ) ? (CACHE_SIZE) : (maxRelSize) ) );  
    radix = ( (radix_t) log2(range) );                                                                  
    return range; 
}

#if defined(__ENABLE_PRINTING_RELATION__)
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
        os << "\n[\"" << relation.name << "\"]: " << relation.size  << " entries...";

        return os;
    }
#endif

RHJ::List RHJ::Relation::RadixHashJoin(const RHJ::Relation& relR, const RHJ::Relation& relS) {

    radix_t radix;
    size_t range = calc_range(relR.size, relS.size, radix);
    PsumTable hashTableR(relR, radix);

    PsumTable hashTableS(relS, radix);

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
