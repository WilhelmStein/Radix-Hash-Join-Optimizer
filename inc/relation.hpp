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

            Tuple() : key(0), payload(0) {}

            friend std::ostream& operator<<(std::ostream&, const Tuple&);
        } * tuples;
        
        std::size_t size;

        Relation(std::size_t size) : tuples(new Tuple[size]), size(size) {}

        ~Relation() { delete[] tuples; }

        friend std::ostream& operator<<(std::ostream&, const Relation&);

        static List RadixHashJoin(const Relation& relR, const Relation& relS);
    };

    std::ostream& operator<<(std::ostream&, const Relation::Tuple&);
    std::ostream& operator<<(std::ostream&, const Relation&);
}
