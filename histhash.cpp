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

    auto cmp_func =  [](const Tuple& a, const Tuple& b)
    {
        if (a.key < b.key)
            return -1;
        else if (a.key == b.key)
            return 0;
        else
            return 1;
    };



    Tuple** arr = tuples.toArray();

    nstd::BinaryTree<Tuple> btree(cmp_func, true);
    
    for(int i = 0; i < tuples.size(); i++) {

        // histogram[i] = new Tuple(0, 0);

        Tuple tuple(arr[i]->payload, 1);
        Tuple* ptr = nullptr;

        if ( ptr = btree.find(tuple) ) {
            ptr->payload++;
        }
        // If it does not exist
        else {
            btree.insert(tuple);
        }        
    }

    // Copy tree to histogram
    int index = 0;
    int size = btree.getSize();
    Tuple *histogram = new Tuple[size];

    while (btree.getRoot()) {
        histogram[index++].key = btree.getRoot()->contents.key;
        histogram[index++].payload = btree.getRoot()->contents.payload;
        btree.removeValue(btree.getRoot()->contents);
    }

    // Create Psum
    Tuple *psum = new Tuple[size];
    // Reusing same btree (it's now devoid of elements)

    int sum = 0;
    for (int i = 0; i < size; i++) {
        psum[i].key = histogram[i].key;
        psum[i].payload = sum;
        Tuple item(psum[i].key, sum);
        btree.insert(item);
        sum += histogram[i].payload;
    }

    // Create R'
    Tuple** arr2 = new Tuple*[tuples.size()];


    for(int i = 0; i < tuples.size(); i++) {
       Tuple* ptr = btree.find(*arr[i]); // Check for error when btree empty;
       arr2[ptr->payload++] = arr[i];
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

