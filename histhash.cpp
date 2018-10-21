#include "histhash.h"

int pow(int a, int e) {
    for(int i = 0; i < e - 1; i++)
        a *= a;
    return a;
}

void Bucket::insert(Tuple* tuple) {
    tuples.append(&tuple);
}



HistHashTable::HistHashTable(const int _n) : n(_n) {
    table = new Bucket[pow(2, n)];
}

HistHashTable::~HistHashTable() {
    delete[] table;
}

int32_t HistHashTable::radixHash(int32_t payload) {
    auto shamt = (sizeof(payload) * 8) - n;
    payload << shamt;
    payload >> shamt;
    
    return payload;
}

void HistHashTable::insert(Tuple* tuple) {
    int32_t hash = this->radixHash(tuple->payload);
}

