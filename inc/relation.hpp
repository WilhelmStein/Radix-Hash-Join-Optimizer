#pragma once

#include <types.hpp>
#include <result.hpp>
#include <iosfwd>               // std::ostream

#if defined (__PSUM_DEBUG__)
    #if !defined(__VERBOSE__)
        #define __VERBOSE__
    #endif
#endif

namespace RHJ
{
    struct Relation {
        #if defined (__VERBOSE__)
            const char * name;
        #endif

        struct Tuple {
            tuple_key_t key;
            tuple_payload_t payload;

            Tuple() : key(0), payload(0) {}

            #if defined (__VERBOSE__)
                friend std::ostream& operator<<(std::ostream&, const Tuple&);
            #endif
        } * tuples;
        
        std::size_t size;

        #if defined (__VERBOSE__)
            Relation(std::size_t size, const char * name) : name(name), tuples(new Tuple[size]), size(size) {}
        #else
            Relation(std::size_t size) : tuples(new Tuple[size]), size(size) {}
        #endif

        ~Relation() { delete[] tuples; }

        friend std::ostream& operator<<(std::ostream&, const Relation&);

        static List RadixHashJoin(const Relation& relR, const Relation& relS);
    };

    #if defined (__VERBOSE__)
        std::ostream& operator<<(std::ostream&, const Relation::Tuple&);
    #endif
    
    std::ostream& operator<<(std::ostream&, const Relation&);
}
