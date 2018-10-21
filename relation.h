#pragma once
#include <stdint.h>

struct Tuple {
    int32_t key;
    int32_t payload;

    Tuple(int32_t _key, int32_t _payload) : key(_key), payload(_payload) {}
};

struct Relation {
    Tuple *tuples;
    uint32_t size;

    Relation(uint32_t _size, Tuple * _tuples) : size(_size), tuples(_tuples) {}
};

struct Result {

};

Result *RadixHashJoin(Relation *relR, Relation *relS);