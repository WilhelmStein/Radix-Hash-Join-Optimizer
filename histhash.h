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
    void sort(const int n);
};

class HistHashTable {
private:
    const int n;
    Bucket *table;

    ~HistHashTable();

    int32_t radixHash(int32_t payload);

public:
    HistHashTable(const int _n);

    void insert(Tuple* tuple);

};