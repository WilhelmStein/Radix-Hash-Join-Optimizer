#pragma once

#include "relation.hpp"
#include "pair.hpp"
#include "types.hpp"

// 0 <= N < 32
#define N (3)

namespace RHJ
{
    class PsumTable {

            Relation table;
            std::size_t * psum;

            const radix_t n;

            std::size_t psum_size;

            std::size_t radixHash(tuple_payload_t value) const;

        public:

            using Bucket = Relation;


            PsumTable(const RHJ::Relation& rel, radix_t _n); 
            ~PsumTable();

            Bucket operator[](std::size_t value) const ;


            void printTable() const ;
            void printPsum() const ;
    };
}
