
#include <histhash.hpp>
#include <thread_pool.hpp>
#include <cmath>            // std::pow
#include <utility>          // std::move
#include <vector>
#include <iostream>

#if defined (__PSUM_DEBUG__)    
    #include <fstream>
#endif

#define HASH(value, radix) (value & ((1UL << radix) - 1UL))

#if defined (__PSUM_DEBUG__)
    std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::PsumTable& psumtable)
    {
        os << "\n<DBG>: psumtable.table.tuples" << std::endl;
        for (std::size_t i = 0UL; i < psumtable.table.size; i++)
            os << psumtable.table.tuples[i] << std::endl;

        os << "\n<DBG>: psumtable.psum" << std::endl;
        for (std::size_t i = 0UL; i < psumtable.psum_size; i++)
            os << i << ":  " << psumtable.psum[i] << std::endl;

        return os;
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

struct HistogramJobContainer {
    RHJ::Relation::Tuple * tuples;
    std::size_t size;

    radix_t radix;
    std::size_t * histogram;
    std::size_t *hashes;
};

void HistogramJob(void * data) {

    HistogramJobContainer *container = (HistogramJobContainer *)data;

    for (std::size_t i = 0; i < container->size; i++) {
        // std::size_t hash = HASH(container->tuples[i].payload, container->radix);
        container->histogram[ container->hashes[i] = HASH(container->tuples[i].payload, container->radix) ]++;
        // container->histogram[ hash ]++;
    }
}

struct PartitionJobContainer {
    RHJ::Relation::Tuple * tuples;
    std::size_t relation_size;

    RHJ::Relation::Tuple * reordered_tuples;
    std::size_t size;

    std::size_t psum_size;
    std::size_t * histogram;
    std::size_t * psum;
    std::size_t *hashes;
};

void PartitionJob(void * data) {
    PartitionJobContainer *container = (PartitionJobContainer *)data;

    // for (int i = 0; i < container->size; i++) {
    //     std::cout << "hashes - " << container->hashes[i] << " ";
    // }
    // std::cout << std::endl;

    for (std::size_t i = 0UL; i < container->size; i++) {

        std::size_t hash = container->hashes[i];

        std::size_t index = container->psum[hash];
        container->psum[hash]++;

        container->reordered_tuples[index] = container->tuples[i];

        container->histogram[hash]--;
    }
}

RHJ::PsumTable::PsumTable(const Relation& rel, radix_t _radix, std::size_t _psum_size) 
:
#if defined (__VERBOSE__)
    table(rel.size, "hashed"), radix(_radix), psum_size(_psum_size), psum(nullptr)
#else
    table(rel.size), radix(_radix), psum_size(_psum_size), psum(nullptr)
#endif
{
    std::size_t *histogram = new std::size_t[psum_size]{0UL};

    // <SINGLE THREAD IMPLEMENTATION> //
    // Creating a table which contains hashes of each tuple
    // std::size_t *hashes = new std::size_t[rel.size];
    // </SINGLE THREAD IMPLEMENTATION> //

    std::size_t num_threads = 16;
    thread_pool::create(num_threads);

    std::size_t curOffset = 0;
    std::size_t offSet = rel.size / num_threads;

    std::vector<HistogramJobContainer *> histogram_containers;

    std::size_t actual_threads = 0;

    for (std::size_t i = 0; i < num_threads; i++) {

        HistogramJobContainer *data = new HistogramJobContainer;

        if (offSet == 0)
            data->size = rel.size;
        else if (i == num_threads - 1) {      
            data->size = rel.size - curOffset;
        }
        else
            data->size = offSet;


        data->radix = this->radix;
        data->histogram = new std::size_t[this->psum_size]{0UL};
        data->tuples = &(rel.tuples[curOffset]);
        data->hashes = new std::size_t[data->size];

        histogram_containers.push_back(data);

        curOffset += (offSet == 0 ? rel.size : offSet);

        thread_pool::schedule(HistogramJob, (void *)(data) );

        actual_threads++;

        if (curOffset >= rel.size) break;
    }

    thread_pool::block();
    
    this->psum = new std::size_t[psum_size];

    std::size_t sum = 0UL;

    for (std::size_t i = 0UL; i < this->psum_size; i++) {
        for (std::size_t j = 0UL; j < histogram_containers.size(); j++) {
            histogram[i] += histogram_containers[j]->histogram[i];
        }
        this->psum[i] = sum;
        sum += histogram[i];
    }
    

    // <SINGLE THREAD IMPLEMENTATION> //
    // for (std::size_t i = 0UL; i < rel.size; i++) {
    //     histogram[ hashes[i] = HASH(rel.tuples[i].payload, radix) ]++;
    // }
    // </SINGLE THREAD IMPLEMENTATION> //

    std::vector<PartitionJobContainer *> partition_containers;

    std::size_t * next_sum = new std::size_t[this->psum_size];
    for (std::size_t j = 0; j < psum_size; j++) {
        next_sum[j] = psum[j];
    }
    
    for (std::size_t i = 0; i < actual_threads; i++) {
        PartitionJobContainer *data = new PartitionJobContainer;

        data->hashes = histogram_containers[i]->hashes;
        data->histogram = histogram_containers[i]->histogram;
        data->psum = next_sum;

        std::size_t *temp_sum = new std::size_t[this->psum_size];
        for (std::size_t j = 0; j < psum_size; j++) {
            temp_sum[j] = next_sum[j] + histogram_containers[i]->histogram[j];
        }

        next_sum = temp_sum;

        data->psum_size = psum_size;
        data->tuples = histogram_containers[i]->tuples;
        data->reordered_tuples = this->table.tuples;
        data->size = histogram_containers[i]->size;
        data->relation_size = rel.size;

        thread_pool::schedule(PartitionJob, (void *)(data) );
        partition_containers.push_back(data);
    }

    thread_pool::block();

    // <SINGLE THREAD IMPLEMENTATION> //
    // for (std::size_t i = 0UL; i < rel.size; i++) {

    //     std::size_t hash = HASH(rel.tuples[i].payload, this->radix);

    //     std::size_t index = (hash < this->psum_size - 1UL ? this->psum[hash + 1UL] : rel.size) - histogram[hash];

    //     this->table.tuples[index] = rel.tuples[i];

    //     histogram[hash]--;

    // }

    // delete[] hashes
    // </SINGLE THREAD IMPLEMENTATION> //

    
    delete[] histogram;
    delete[] next_sum;

    for (std::size_t i = 0; i < actual_threads; i++) {
        delete[] partition_containers[i]->hashes;
        delete[] partition_containers[i]->histogram;
        delete[] partition_containers[i]->psum;
        delete partition_containers[i];
        delete histogram_containers[i];
    }

    thread_pool::destroy();
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