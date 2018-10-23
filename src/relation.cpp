#include "relation.hpp"
#include "iostream"
#include "histhash.hpp"
#include <cstdint>

#define N (3)

void RHJ::Tuple::print() const {
    std::cout << "key: " << this->key << ", value: " << this->payload << std::endl;
}


RHJ::Result *RHJ::Relation::RadixHashJoin(RHJ::Relation *relR, RHJ::Relation *relS) {

    PsumTable hashTableR(*relR, N);

    PsumTable hashTableS(*relS, N);

    PsumTable::Bucket bucket = hashTableR[3];

    for (uint32_t i = 0U; i < bucket.size; i++) {
        bucket.tuples[i].print();
    }
}

