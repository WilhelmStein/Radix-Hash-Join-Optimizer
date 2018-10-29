#include <relation.hpp>
#include <histhash.hpp>
#include <index.hpp>
#include <iostream>

#define RADIX (3)
#define RANGE (2 * 2 * 2)

void RHJ::Relation::Tuple::print() const {
    std::cout << "key: " << this->key << ", value: " << this->payload << std::endl;
}


RHJ::List *RHJ::Relation::RadixHashJoin(RHJ::Relation *relR, RHJ::Relation *relS) {

    PsumTable hashTableR(*relR, RADIX);

    PsumTable hashTableS(*relS, RADIX);

    List * const results = new List;
    for (std::size_t hash = 0UL; hash < RANGE; hash++)
    {
        PsumTable::Bucket r = hashTableR[hash];
        PsumTable::Bucket s = hashTableS[hash];

        if (!r.size || !s.size)
            continue;

        if (r.size < s.size)
        {
            Index index(r); index.join(s, *results);
        }
        else
        {
            Index index(s); index.join(r, *results);
        }
    }

    return results;
}
