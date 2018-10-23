#include "histhash.h"
#include <iostream>

int pow(int a, int e) {
    int ret = a;
    for (int i = 0; i < e - 1; i++)
        ret *= a;
    return ret;
}

int32_t PsumTable::radixHash(int32_t value) {
    return (value & ((1 << this->n) - 1));
}

void PsumTable::printTable() {
    for (int i = 0; i < this->table_size; i++) {
        this->table[i].print();
    }
}

void PsumTable::printPsum() {
    for (int i = 0; i < this->psum_size; i++) {
        std::cout << i << ":  " << this->psum[i] << std::endl;
    }
}


PsumTable::PsumTable(Relation *rel, uint32_t n) {

    this->n = n;
    this->psum_size = pow(2, this->n);

    int histogram[psum_size] = { 0 };

    // Creating a table which contains hashes of each tuple
    uint32_t hashes[rel->size];

    for (int i = 0; i < rel->size; i++) {
        int32_t hash = radixHash(rel->tuples[i].payload);

        hashes[i] = hash;

        histogram[hash]++;
    }

    this->psum = new int[psum_size];
    // int psum_2[psum_size];
    int sum = 0;

    for (int i = 0; i < psum_size; i++) {
        this->psum[i] = sum;
        // psum_2[i] = sum;
        sum += histogram[i];
    }

    this->table = (Tuple *)malloc(rel->size * sizeof(Tuple));

    for (int i = 0; i < rel->size; i++) {
        int32_t hash = hashes[i];

        uint32_t index = (hash < this->psum_size - 1 ? this->psum[hash + 1] : rel->size) - histogram[hash];

        this->table[ index ] = rel->tuples[i];

        histogram[hash]--;

    }
    
    
    // this->psum = psum;
    this->table_size = rel->size;
}

PsumTable::~PsumTable() {
    delete[] psum;
}

// Returns Bucket with hash same as hashed(value)
// So if we give value = 10 and n = 2 it will return the bucket with hash = hashed(10) = 0b10 = 2
PsumTable::Result PsumTable::operator[](int32_t value) {
    int returnSize;

    int32_t hash = radixHash(value);

    if (hash < this->psum_size) {
        returnSize = this->psum[hash + 1] - this->psum[hash];
    }
    else {
        returnSize = this->table_size - this->psum[hash];
    }

    return Result(&this->table[this->psum[hash]], returnSize);

    // Tuple * returnTable = (Tuple *)malloc(returnSize * sizeof(Tuple));

    // for (int i = 0; i < returnSize; i++) {
    //     returnTable[i] = this->table[ this->psum[hash] + i ];
    // }

    // return PsumTableResult(returnTable, returnSize);
}