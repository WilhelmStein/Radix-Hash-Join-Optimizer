#pragma once

#include <types.hpp>
#include <relation.hpp>

namespace RHJ
{
    class PsumTable {

            Relation table;

            const radix_t radix;

            std::size_t psum_size;
            std::size_t * psum;

        public:

            using Bucket = Relation;


            PsumTable(const RHJ::Relation& rel, radix_t _n); 
            ~PsumTable();

            Bucket operator[](std::size_t value) const ;

            #ifdef __DEBUG_PSUM__
                void printTable() const ;
                void printPsum() const ;
            #endif
    };
}
