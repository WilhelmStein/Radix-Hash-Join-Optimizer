#pragma once

#include "types.hpp"
#include "pair.hpp"
#include <cstdint>

struct Tuple {
    int32_t key;
    int32_t payload;

    Tuple(int32_t _key, int32_t _payload) : key(_key), payload(_payload) {}
    Tuple() : key(0), payload(0) {};
    void print();
};

struct Relation {
    Tuple *tuples;
    uint32_t size;

    Relation(uint32_t _size, Tuple * _tuples) : tuples(_tuples), size(_size) {}
};

using Result = pair<relation_size_t, relation_size_t>;

Result *RadixHashJoin(Relation *relR, Relation *relS);
