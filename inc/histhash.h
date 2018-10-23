#pragma once
#include "relation.h"
#include "pair.hpp"

#define N (3)

int pow(int a, int e);

// int32_t radixHash(uint32_t n, int32_t value);


class PsumTable {

    private:
        Tuple * table;
        int * psum;

        uint32_t n;
        int psum_size;
        int table_size;

        int32_t radixHash(int32_t value);

    public:
        using Result = nstd::pair<Tuple *, uint32_t>;

        PsumTable(Relation *rel, uint32_t n);
        ~PsumTable();

        Result getBucket(int32_t value);

        void printTable();
        void printPsum();
};

