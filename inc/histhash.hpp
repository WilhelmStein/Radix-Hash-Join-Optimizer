#pragma once

#include <types.hpp>
#include <relation.hpp>
#include <iosfwd>

namespace RHJ
{
    class PsumTable {
            
            friend class Index;

            Relation table;

            const radix_t radix;

            std::size_t psum_size;
            std::size_t * psum;

        public:

            struct Bucket
            {
                Relation::Tuple * tuples;
                std::size_t size;

                Bucket(Relation::Tuple *, std::size_t);
                Bucket(Bucket&&) noexcept;

                Bucket& operator=(Bucket&&) noexcept;
            };

            PsumTable(const RHJ::Relation& rel, radix_t _radix, std::size_t _range); 
            ~PsumTable();

            Bucket operator[](std::size_t value) const ;

            #if defined(__DEBUG_PSUM__)
                friend std::ostream& operator<<(std::ostream&, const PsumTable&);
            #endif
    };

    #if defined(__DEBUG_PSUM__)
        std::ostream& operator<<(std::ostream&, const PsumTable&);
    #endif
}
