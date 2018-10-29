#pragma once

#include <types.hpp>
#include <result.hpp>
#include <iosfwd>

namespace RHJ
{
    struct Relation {
        struct Tuple {
            tuple_key_t key;
            tuple_payload_t payload;

            friend std::ostream& operator<<(std::ostream&, const Tuple&);
        } * tuples;
        
        std::size_t size;

        Relation();
        Relation(Tuple *, std::size_t);
        Relation(Relation&&) noexcept;

        ~Relation();

        Relation& operator=(Relation&&) noexcept;

        friend std::ostream& operator<<(std::ostream&, const Relation&);

        static List RadixHashJoin(const Relation& relR, const Relation& relS);
    };

    std::ostream& operator<<(std::ostream&, const Relation::Tuple&);
    std::ostream& operator<<(std::ostream&, const Relation&);
}
