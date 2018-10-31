#include <histhash.hpp>
#include <cmath>            // std::pow
#include <utility>          // std::move

#define HASH(value, radix) (value & ((1UL << radix) - 1UL))

#if defined(__ENABLE_PRINTING_PSUMTABLE__)
    #include <iostream>

    void RHJ::PsumTable::printTable() const {
        for (std::size_t i = 0UL; i < table.size; i++)
            std::cerr << table.tuples[i] << std::endl;
    }

    void RHJ::PsumTable::printPsum() const {
        for (std::size_t i = 0UL; i < psum_size; i++)
            std::cerr << i << ":  " << psum[i] << std::endl;
    }
#endif

RHJ::PsumTable::Bucket::Bucket(Relation::Tuple * tuples, std::size_t size)
:
tuples(tuples),
size(size)
{
}

RHJ::PsumTable::Bucket::Bucket(Bucket&& other) noexcept
:
tuples(std::move(other.tuples)),
size(std::move(other.size))
{
}

RHJ::PsumTable::Bucket& RHJ::PsumTable::Bucket::operator=(Bucket&& other) noexcept
{
    tuples = std::move(other.tuples);
    size   = std::move(other.size);

    return *this;
}

RHJ::PsumTable::PsumTable(const Relation& rel, radix_t radix) 
: 
table(rel.size), radix(radix), psum_size(0UL), psum(nullptr)
{
    this->psum_size = std::pow(2UL, static_cast<uint64_t>(radix));

    std::size_t *histogram = new std::size_t[psum_size]{0UL};

    // Creating a table which contains hashes of each tuple
    std::size_t *hashes = new std::size_t[rel.size];


    for (std::size_t i = 0UL; i < rel.size; i++)
        histogram[ hashes[i] = HASH(rel.tuples[i].payload, radix) ]++;


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
// So if we give value = 10 and radix = 2 it will return the bucket with hash = hashed(10) = 0b10 = 2
RHJ::PsumTable::Bucket RHJ::PsumTable::operator[](std::size_t hash) const {

    return {
        &table.tuples[psum[hash]],
        ( hash < psum_size - 1UL ? psum[hash + 1UL] : table.size ) - psum[hash] 
    };
}