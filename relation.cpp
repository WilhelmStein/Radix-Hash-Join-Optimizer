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

    PsumTable::Result result = hashTableR.getBucket(3);

    for (int i = 0; i < result.second; i++) {
        result.first[i].print();
    }

}

