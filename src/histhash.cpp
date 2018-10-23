#include "histhash.hpp"
#include <iostream>


std::size_t RHJ::PsumTable::radixHash(tuple_payload_t value) const {
    return (value & ((1UL << this->n) - 1UL));
}

void RHJ::PsumTable::printTable() const {
    for (std::size_t i = 0UL; i < this->table.size; i++) {
        this->table.tuples[i].print();
    }
}

void RHJ::PsumTable::printPsum() const {
    for (std::size_t i = 0UL; i < this->psum_size; i++) {
        std::cout << i << ":  " << this->psum[i] << std::endl;
    }
}

RHJ::PsumTable::PsumTable(const Relation& rel, radix_t _n) 
: 
table({ new Tuple[rel.size], rel.size }), n(_n)
{ 
    // 2 ^ n
    this->psum_size = [this]()
    {
        std::size_t ret = 2UL;
        for (radix_t i = 0U; i < n - 1U; i++)
            ret *= 2UL;
        
        return ret;
    }();

    std::size_t *histogram = new std::size_t[psum_size];

    // Creating a table which contains hashes of each tuple
    std::size_t *hashes = new std::size_t[rel.size];


    for (std::size_t i = 0UL; i < rel.size; i++)
        histogram[ hashes[i] = radixHash(rel.tuples[i].payload) ]++;


    this->psum = new std::size_t[psum_size];

    std::size_t sum = 0UL;

    for (std::size_t i = 0UL; i < psum_size; i++) {
        this->psum[i] = sum;
        sum += histogram[i];
    }

    for (std::size_t i = 0UL; i < rel.size; i++) {

        std::size_t hash = hashes[i];

        std::size_t index = (hash < this->psum_size - 1UL ? this->psum[hash + 1UL] : rel.size) - histogram[hash];

        this->table.tuples[index] = rel.tuples[i];

        histogram[hash]--;

    }

    delete[] histogram;
    delete[] hashes;
}

RHJ::PsumTable::~PsumTable() {
    delete[] psum;
}

// Returns Bucket with hash same as hashed(value)
// So if we give value = 10 and n = 2 it will return the bucket with hash = hashed(10) = 0b10 = 2
RHJ::PsumTable::Bucket RHJ::PsumTable::operator[](std::size_t value) const {

    std::size_t hash = radixHash(value);

    return { 
        &this->table.tuples[this->psum[hash]], 
        ( hash < this->psum_size ? this->psum[hash + 1] : this->table.size ) - this->psum[hash] 
    };
}