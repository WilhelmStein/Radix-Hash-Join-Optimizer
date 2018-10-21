#include "histhash.h"
#include "nbinary_tree.h"

int pow(int a, int e) {
    for(int i = 0; i < e - 1; i++)
        a *= a;
    return a;
}

void Bucket::insert(Tuple* tuple) {
    tuples.append(&tuple);
}

void Bucket::sort(const int n) {
    Tuple** arr = tuples.toArray();
    Tuple** histogram = new Tuple*[pow(2, n)];

    nstd::BinaryTree<Tuple *> btree
    (
        [](const Tuple *&, const Tuple *&)
        {
            if (a->key < b->key)
                return -1;
            else if (a->key == b->key)
                return 0;
            else
                return 1;
        },
        true
    );
    
    for(int i = 0; i < tuples.size(); i++) {

        histogram[i] = new Tuple(0, 0);
        
        // arr[i]->payload
    }
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
    table[hash].insert(tuple);
}

