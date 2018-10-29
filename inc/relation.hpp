#pragma once

#include <types.hpp>
#include <result.hpp>

namespace RHJ
{
    struct Relation {
        struct Tuple {
            tuple_key_t key;
            tuple_payload_t payload;

            Tuple(tuple_key_t _key, tuple_payload_t _payload) : key(_key), payload(_payload) {}
            Tuple() : key(0), payload(0) {};

            void print() const ;
        } * tuples;
        
        std::size_t size;

        static List *RadixHashJoin(Relation *relR, Relation *relS);
    };
}
