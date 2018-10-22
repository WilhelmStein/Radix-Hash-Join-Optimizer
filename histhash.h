#pragma once
#include "relation.h"

#define N (3)

int pow(int a, int e);

int32_t radixHash(uint32_t n, int32_t value);

struct PsumTableResult {
    int size;
    Tuple * tuples;

    PsumTableResult(Tuple * _tuples, int _size) : size(_size), tuples(_tuples) {};
    void print();
};


class PsumTable {

    private:
        Tuple * table;
        int * psum;

        int n;
        int psum_size;
        int table_size;

        int32_t radixHash(uint32_t n, int32_t value);

    public:
        PsumTable(Relation *rel);
        ~PsumTable();

        PsumTableResult getBucket(int32_t value);

        void printTable();
        void printPsum();
};

