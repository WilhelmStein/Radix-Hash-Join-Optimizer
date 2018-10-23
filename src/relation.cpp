#include "relation.h"
#include "iostream"
#include "histhash.h"
#include <stdint.h>

#define N (3)

void Tuple::print() {
    std::cout << "key: " << this->key << ", value: " << this->payload << std::endl;
}


Result *RadixHashJoin(Relation *relR, Relation *relS) {
    PsumTable hashTableR(relR, N);

    PsumTable hashTableS(relS, N);

    PsumTable::Result result = hashTableR[3];

    for (uint32_t i = 0U; i < result.second; i++) {
        result.first[i].print();
    }

}

