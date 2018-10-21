#pragma once
#include "relation.h"
#include "list.h"

class Bucket {
private:
    nstd::List<Tuple *> tuples;
public:
    Bucket() {}
    ~Bucket() {}

    void insert(Tuple* tuple);
};

class HistHashTable {
private:
    const int n;
    Bucket *table;
    ~HistHashTable();
public:
    HistHashTable(const int _n);
    int32_t radixHash(int32_t payload);
    void insert(Tuple* tuple);

};