#pragma once
#include "relation.h"
#include "pair.hpp"

#define N (3)

int pow(int a, int e);

class PsumTable {

    private:
        Tuple * table;
        int * psum;

        uint32_t n;
        int psum_size;
        int table_size;

        int32_t radixHash(int32_t value);

    public:
        using Result = pair<Tuple *, uint32_t>;


        PsumTable(Relation *rel, uint32_t n);
        ~PsumTable();

        Result operator[](int32_t value);


        void printTable();
        void printPsum();
};

