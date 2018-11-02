#pragma once

#include <types.hpp>
#include <result.hpp>
#include <iosfwd>

namespace RHJ
{
    struct Relation {
        #if defined (__ENABLE_PRINTING_RELATION__)
            const char * name;
        #endif

        struct Tuple {
            tuple_key_t key;
            tuple_payload_t payload;

            Tuple() : key(0), payload(0) {}

            #if defined(__ENABLE_PRINTING_RELATION__)
                friend std::ostream& operator<<(std::ostream&, const Tuple&);
            #endif
        } * tuples;
        
        std::size_t size;

        #if defined (__ENABLE_PRINTING_RELATION__)
            Relation(std::size_t size, const char * name = "Placeholder") : name(name), tuples(new Tuple[size]), size(size) {}
        #else
            Relation(std::size_t size) : tuples(new Tuple[size]), size(size) {}
        #endif

        ~Relation() { delete[] tuples; }

        #if defined(__ENABLE_PRINTING_RELATION__)
            friend std::ostream& operator<<(std::ostream&, const Relation&);
        #endif

        static List RadixHashJoin(const Relation& relR, const Relation& relS);
    };

    #if defined(__ENABLE_PRINTING_RELATION__)
        std::ostream& operator<<(std::ostream&, const Relation::Tuple&);
        std::ostream& operator<<(std::ostream&, const Relation&);
    #endif
}
