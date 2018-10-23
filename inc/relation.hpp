#pragma once

#include "types.hpp"
#include "pair.hpp"
#include <cstdint>

namespace RHJ
{
    struct Tuple {
        tuple_key_t key;
        tuple_payload_t payload;

        Tuple(tuple_key_t _key, tuple_payload_t _payload) : key(_key), payload(_payload) {}
        Tuple() : key(0), payload(0) {};

        void print() const ;
    };

    struct Result {
        tuple_key_t key1;
        tuple_key_t key2;
    };

    struct Relation {
        Tuple *tuples;
        std::size_t size;

        static Result *RadixHashJoin(Relation *relR, Relation *relS);
    };
}
