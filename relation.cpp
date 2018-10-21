#include "relation.h"
#include "histhash.h"
#include "iostream"

void Tuple::print() {
    std::cout << "key: " << this->key << ", value: " << this->payload << std::endl;
}

Result *RadixHashJoin(Relation *relR, Relation *relS) {
    // Hash R
    HistHashTable R_HashTable(2);

    for (int i = 0; i < relR->size; i++) {
        R_HashTable.insert( &(relR->tuples[i]) );
    }

    R_HashTable.sort();

}