#include "relation.h"
#include "iostream"
#include "histhash.h"
#include <stdint.h>

void Tuple::print() {
    std::cout << "key: " << this->key << ", value: " << this->payload << std::endl;
}


Result *RadixHashJoin(Relation *relR, Relation *relS) {
    PsumTable hashTableR(relR);

    PsumTable hashTableS(relS);

    PsumTableResult result = hashTableR.getBucket(134);
    result.print();
}

